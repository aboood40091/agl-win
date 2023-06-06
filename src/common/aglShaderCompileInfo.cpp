#include <common/aglShaderCompileInfo.h>
#include <common/aglShaderEnum.h>
#include <detail/aglShaderTextUtil.h>
#include <detail/aglPrivateResource.h>

#include <cstring>

namespace agl {

ShaderCompileInfo::ShaderCompileInfo()
    : INamable("unititled")
    , mSourceText(nullptr)
    , mRawText(nullptr)
    , mMacroName()
    , mMacroValue()
    , mVariationName()
    , mVariationValue()
    , _40()
    , _48()
{
}

ShaderCompileInfo::~ShaderCompileInfo()
{
    destroy();
}

void ShaderCompileInfo::create(s32 num_macro, s32 num_variation)
{
    if (num_macro > 0)
    {
        mMacroName.allocBuffer(num_macro);
        mMacroValue.allocBuffer(num_macro);
    }

    if (num_variation > 0)
    {
        mVariationName.allocBuffer(num_variation);
        mVariationValue.allocBuffer(num_variation);
    }
}

void ShaderCompileInfo::clearVariation()
{
    mVariationName.clear();
    mVariationValue.clear();
}

void ShaderCompileInfo::pushBackVariation(const char* name, const char* value)
{
    mVariationName.pushBack(name);
    mVariationValue.pushBack(value);
}

void ShaderCompileInfo::calcCompileSource(ShaderType type, std::string* p_buffer, Target target, bool) const
{
    RIO_ASSERT(p_buffer != nullptr);

    p_buffer->operator=("");

    if (!mSourceText)
        return;

    const char* text = mSourceText->c_str();

    bool is_gl = target == cTarget_GL;

    const char* version_pos = std::strstr(text, "#version");
    if (version_pos)
    {
        s32 line_feed_pos, line_feed_len;

        line_feed_pos = detail::ShaderTextUtil::findLineFeedCode(version_pos, &line_feed_len);
        if (line_feed_pos != -1)
            text = version_pos + line_feed_pos + line_feed_len;
    }

    static const char* sMacroDefine[] = {
        // GL
        "#version 400\n" \
        "#extension GL_ARB_texture_cube_map_array : enable\n" \
        "#extension GL_ARB_shading_language_420pack : enable\n",

        // GX2
        "#version 330\n" \
        "#extension GL_ARB_texture_cube_map_array : enable\n"
    };

    static const char* sTargetDefine[] = {
        "#define AGL_TARGET_GL \n",
        "#define AGL_TARGET_GX2 \n"
    };

    static const char* sTypeDefine[cShaderType_Num] = {
        "#define AGL_VERTEX_SHADER \n",
        "#define AGL_FRAGMENT_SHADER \n",
        "#define AGL_GEOMETRY_SHADER \n"
    };

    p_buffer->operator+=(sMacroDefine[is_gl ? 0 : 1]);
    p_buffer->operator+=("// ----- These macros are auto defined by AGL.-----\n");
    p_buffer->operator+=(sTypeDefine[type]);
    p_buffer->operator+=(sTargetDefine[is_gl ? 0 : 1]);
    p_buffer->operator+=("// ------------------------------------------------\n");
    p_buffer->operator+=(text);

    if (mMacroName.size() > 0)
    {
        detail::ShaderTextUtil::replaceMacro(
            p_buffer,
            static_cast<const char**>(mMacroName.getWork()),
            static_cast<const char**>(mMacroValue.getWork()),
            mMacroName.size(),
            detail::PrivateResource::sShaderWorkBuffer.getBufferPtr(),
            detail::PrivateResource::sShaderWorkBuffer.size()
        );
    }

    if (mVariationName.size() > 0)
    {
        detail::ShaderTextUtil::replaceMacro(
            p_buffer,
            static_cast<const char**>(mVariationName.getWork()),
            static_cast<const char**>(mVariationValue.getWork()),
            mVariationName.size(),
            detail::PrivateResource::sShaderWorkBuffer.getBufferPtr(),
            detail::PrivateResource::sShaderWorkBuffer.size()
        );
    }

    if (mRawText)
        mRawText->operator=(*p_buffer);
}

void ShaderCompileInfo::destroy()
{
    if (mMacroName.isBufferReady())
    {
        mMacroName.freeBuffer();

        RIO_ASSERT(mMacroValue.isBufferReady());
        mMacroValue.freeBuffer();
    }

    if (mVariationName.isBufferReady())
    {
        mVariationName.freeBuffer();

        RIO_ASSERT(mVariationValue.isBufferReady());
        mVariationValue.freeBuffer();
    }
}

}
