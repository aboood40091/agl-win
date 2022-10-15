#include <detail/aglGX2.h>

#include <misc/rio_MemUtil.h>

#if RIO_IS_CAFE
#include <cafe.h>
#endif // RIO_IS_CAFE

namespace agl { namespace driver {

GX2Resource* GX2Resource::sInstance = NULL;

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
    , mGeometryShaderInputRingBuffer(NULL)
    , mGeometryShaderInputRingBufferSize(0)
    , mGeometryShaderOutputRingBuffer(NULL)
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
        mGeometryShaderInputRingBuffer = NULL;
    }

    if (mGeometryShaderOutputRingBuffer)
    {
        rio::MemUtil::free(mGeometryShaderOutputRingBuffer);
        mGeometryShaderOutputRingBuffer = NULL;
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
        GX2Invalidate(GX2_INVALIDATE_UNIFORM_BLOCK, NULL, 0xFFFFFFFF);
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
        GX2Invalidate(GX2_INVALIDATE_UNIFORM_BLOCK, NULL, 0xFFFFFFFF);
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

/*
void GX2Resource::restoreContextState()
{
    if (mStateShadowEnable)
    {
        GX2ContextState* state = sead::GraphicsCafe::instance()->getContextState();

        if (!mUseStateDisplayList)
            GX2SetContextState(state);

        else
        {
            void* p_dl;
            u32 size;
            // Either this function was inline back in the GX2 headers NSMBU used or they had a separate function to do this
            GX2GetContextStateDisplayList(state, &p_dl, &size);

            GX2CallDisplayList(p_dl, size);
        }
    }
    else
    {
        GX2SetContextState(NULL);
    }
}
*/

} }
