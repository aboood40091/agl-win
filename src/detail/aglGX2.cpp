#include <detail/aglGX2.h>
#include <misc/rio_MemUtil.h>

#if RIO_IS_CAFE
#include <gfx/rio_Window.h>

#include <cafe.h>
#endif // RIO_IS_CAFE

namespace agl { namespace driver {

GX2Resource* GX2Resource::sInstance = nullptr;

bool GX2Resource::createSingleton()
{
    if (sInstance)
        return false;

    sInstance = new GX2Resource();
    return true;
}

void GX2Resource::destroySingleton()
{
    if (!sInstance)
        return;

    delete sInstance;
    sInstance = nullptr;
}

GX2Resource::GX2Resource()
    : mpOptimizeInfo(&mDefaultOptimizeInfo)
    , mDefaultOptimizeInfo()
    , mGeometryShaderInputRingItemSize(0)
    , mGeometryShaderOutputRingItemSize(0)
    , mGeometryShaderInputRingBuffer(nullptr)
    , mGeometryShaderInputRingBufferSize(0)
    , mGeometryShaderOutputRingBuffer(nullptr)
    , mGeometryShaderOutputRingBufferSize(0)
    , mStateShadowEnable(true)
    , mUseStateDisplayList(false)
{
  //detail::RootNode::setNodeMeta("Icon = CIRCLE_BLUE");
}

GX2Resource::~GX2Resource()
{
    if (mGeometryShaderInputRingBuffer)
    {
        rio::MemUtil::free(mGeometryShaderInputRingBuffer);
        mGeometryShaderInputRingBuffer = nullptr;
    }

    if (mGeometryShaderOutputRingBuffer)
    {
        rio::MemUtil::free(mGeometryShaderOutputRingBuffer);
        mGeometryShaderOutputRingBuffer = nullptr;
    }
}

void GX2Resource::initialize(u32 gs_in_ring_item_size, u32 gs_out_ring_item_size)
{
    mGeometryShaderOutputRingItemSize = gs_out_ring_item_size;
    mGeometryShaderInputRingItemSize = gs_in_ring_item_size;

#if RIO_IS_CAFE
    mGeometryShaderInputRingBufferSize = GX2CalcGeometryShaderInputRingBufferSize(mGeometryShaderInputRingItemSize);
    mGeometryShaderOutputRingBufferSize = GX2CalcGeometryShaderOutputRingBufferSize(mGeometryShaderOutputRingItemSize);

    if (mGeometryShaderInputRingBufferSize > 0)
        mGeometryShaderInputRingBuffer = static_cast<u8*>(rio::MemUtil::alloc(mGeometryShaderInputRingBufferSize, 0x100));

    if (mGeometryShaderOutputRingBufferSize > 0)
        mGeometryShaderOutputRingBuffer = static_cast<u8*>(rio::MemUtil::alloc(mGeometryShaderOutputRingBufferSize, 0x100));

    if (mGeometryShaderInputRingBuffer)
        DCFlushRange(mGeometryShaderInputRingBuffer, mGeometryShaderInputRingBufferSize);

    if (mGeometryShaderOutputRingBuffer)
        DCFlushRange(mGeometryShaderOutputRingBuffer, mGeometryShaderOutputRingBufferSize);
#endif // RIO_IS_CAFE
}

void GX2Resource::setShaderMode(ShaderMode mode) const
{
    setShaderMode(mode, *mpOptimizeInfo);
}

void GX2Resource::setShaderMode(ShaderMode mode, const ShaderOptimizeInfo& info) const
{
#if RIO_IS_CAFE
    switch (mode)
    {
    case cShaderMode_UniformRegister:
        GX2SetShaderModeEx(
            GX2_SHADER_MODE_UNIFORM_REGISTER,
            info.getVertexShaderGprs(),
            info.getVertexShaderStackSize(),
            0,
            0,
            info.getFragmentShaderGprs(),
            info.getFragmentShaderStackSize()
        );
        break;
    case cShaderMode_UniformBlock:
        GX2SetShaderModeEx(
            GX2_SHADER_MODE_UNIFORM_BLOCK,
            info.getVertexShaderGprs(),
            info.getVertexShaderStackSize(),
            0,
            0,
            info.getFragmentShaderGprs(),
            info.getFragmentShaderStackSize()
        );
        GX2Invalidate(GX2_INVALIDATE_UNIFORM_BLOCK, nullptr, 0xFFFFFFFF);
        break;
    case cShaderMode_GeometryShader:
        GX2SetShaderModeEx(
            GX2_SHADER_MODE_GEOMETRY_SHADER,
            info.getVertexShaderWithGSGprs(),
            info.getVertexShaderWithGSStackSize(),
            info.getGeometryShaderGprs(),
            info.getGeometryShaderStackSize(),
            info.getFragmentShaderWithGSGprs(),
            info.getFragmentShaderWithGSStackSize()
        );
        GX2Invalidate(GX2_INVALIDATE_UNIFORM_BLOCK, nullptr, 0xFFFFFFFF);
        break;
    }
#endif // RIO_IS_CAFE
}

void GX2Resource::setGeometryShaderRingBuffer() const
{
#if RIO_IS_CAFE
    GX2SetGeometryShaderInputRingBuffer(mGeometryShaderInputRingBuffer, mGeometryShaderInputRingBufferSize);
    GX2SetGeometryShaderOutputRingBuffer(mGeometryShaderOutputRingBuffer, mGeometryShaderOutputRingBufferSize);
#endif // RIO_IS_CAFE
}

#if RIO_IS_CAFE

namespace {

inline void GetContextStateDisplayList(GX2ContextState* state, void** pp_dl, u32* p_size)
{
    // Either this function was inline back in the GX2 headers NSMBU used...
    // GX2GetContextStateDisplayList(state, pp_dl, p_size);

    // ... Or they did this
    *pp_dl  = state->shadowDisplayList;
    *p_size = state->shadowDisplayListSize;
}

}

void GX2Resource::restoreContextState()
{
    if (mStateShadowEnable)
    {
        GX2ContextState* state = rio::Window::instance()->getNativeWindow().p_context_state;

        if (!mUseStateDisplayList)
            GX2SetContextState(state);

        else
        {
            void* p_dl;
            u32 size;
            GetContextStateDisplayList(state, &p_dl, &size);

            GX2CallDisplayList(p_dl, size);
        }
    }
    else
    {
        GX2SetContextState(nullptr);
    }
}

#endif // RIO_IS_CAFE

} }
