#include <codec/HashCRC32.h>
#include <misc/rio_MemUtil.h>
#include <utility/aglParameter.h>
#include <utility/aglParameterObj.h>

#include <cstring>

namespace agl { namespace utl {

ParameterBase::ParameterBase()
{
    initializeListNode("default", "parameter", "", nullptr);
}

void ParameterBase::applyResource(ResParameter res)
{
    if (getParameterType() != cType_bool)
        rio::MemUtil::copy(ptr(), res.getValue(), size());
    else
        *static_cast<bool*>(ptr()) = *static_cast<const u8*>(res.getValue()) != 0; // Literally only this line doesn't match

    postApplyResource_(res.getValue(), res.ptr()->mSize - sizeof(ResParameterData));
}

void ParameterBase::applyResource(ResParameter res, f32 t)
{
    switch (getParameterType())
    {
    case cType_bool:
        *static_cast<bool*>(ptr()) = *static_cast<const u8*>(res.getValue()) != 0; // Same issue as function above
        break;
    case cType_f32:
        {
            f32 temp;
            rio::MemUtil::copy(&temp, res.getValue(), size());
            *static_cast<f32*>(ptr()) += temp * t;
        }
        break;
    case cType_vec2:
        {
            rio::Vector2f temp;
            rio::MemUtil::copy(&temp, res.getValue(), size());
            *static_cast<rio::Vector2f*>(ptr()) += temp * t;
        }
        break;
    case cType_vec3:
        {
            rio::Vector3f temp;
            rio::MemUtil::copy(&temp, res.getValue(), size());
            *static_cast<rio::Vector3f*>(ptr()) += temp * t;
        }
        break;
    case cType_vec4:
        {
            rio::Vector4f temp;
            rio::MemUtil::copy(&temp, res.getValue(), size());
            *static_cast<rio::Vector4f*>(ptr()) += temp * t;
        }
        break;
    case cType_color:
        {
            rio::Color4f temp;
            rio::MemUtil::copy(&temp, res.getValue(), size());
            *static_cast<rio::Color4f*>(ptr()) += temp * rio::Color4f{t, t, t, t};
        }
        break;
    case cType_int:
    case cType_string32:
    case cType_string64:
        rio::MemUtil::copy(ptr(), res.getValue(), size());
        break;
    case cType_curve1:
    case cType_curve2:
    case cType_curve3:
    case cType_curve4:
        break;
    default:
        RIO_LOG("%d\n", s32(getParameterType()));
        RIO_ASSERT(false);
    }

    postApplyResource_(res.getValue(), res.ptr()->mSize - sizeof(ResParameterData));
}

bool ParameterBase::copy(const ParameterBase& parameter)
{
    if (getParameterType() != parameter.getParameterType() ||
        mHash != parameter.mHash)
    {
        return false;
    }

    copyUnsafe(parameter);
    return true;
}

void ParameterBase::copyUnsafe(const ParameterBase& parameter)
{
    u8* dst = static_cast<u8*>(ptr());
    const u8* src = static_cast<const u8*>(parameter.ptr());
    s32 len = size();

    for (s32 i = 0; i < len; i++)
        *dst++ = *src++;
}

bool ParameterBase::copyLerp(const ParameterBase& parameter_a, const ParameterBase& parameter_b, f32 t)
{
    if (getParameterType() != parameter_a.getParameterType() ||
        mHash != parameter_a.mHash ||
        getParameterType() != parameter_b.getParameterType() ||
        mHash != parameter_b.mHash)
    {
        return false;
    }

    switch (getParameterType())
    {
    case cType_bool:
    case cType_int:
    case cType_string32:
    case cType_string64:
        copyUnsafe(parameter_a);
        break;
    case cType_f32:
        copyLerp_<f32>(parameter_a, parameter_b, t);
        break;
    case cType_vec2:
        copyLerp_<rio::Vector2f>(parameter_a, parameter_b, t);
        break;
    case cType_vec3:
        copyLerp_<rio::Vector3f>(parameter_a, parameter_b, t);
        break;
    case cType_vec4:
        copyLerp_<rio::Vector4f>(parameter_a, parameter_b, t);
        break;
    case cType_color:
        copyLerp_<rio::Color4f>(parameter_a, parameter_b, t);
        break;
    case cType_curve1:
    case cType_curve2:
    case cType_curve3:
    case cType_curve4:
        break;
    default:
        RIO_LOG("%d\n", s32(getParameterType()));
        RIO_ASSERT(false);
    }
    return true;
}

void ParameterBase::initializeListNode(const char* name, const char* label, const char* meta, IParameterObj* p_obj)
{
    mHash = calcHash(name);

    if (p_obj)
        p_obj->pushBackListNode(this);
}

u32 ParameterBase::calcHash(const char* s)
{
    return HashCRC32::calcHash(s, std::strlen(s));
}

} }
