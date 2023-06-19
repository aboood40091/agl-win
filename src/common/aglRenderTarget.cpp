#include <common/aglRenderTarget.h>
#include <misc/rio_MemUtil.h>

#if RIO_IS_CAFE
#include <gx2/mem.h>
#endif // RIO_IS_CAFE

namespace agl {

void RenderTargetColor::onApplyTextureData_()
{
#if RIO_IS_CAFE
  //mIsMSAA2D = mTextureData.getTextureType() == cTextureType_2D_MSAA;

    mInnerBuffer.surface = mTextureData.getSurface();
    mInnerBuffer.surface.use = GX2_SURFACE_USE_COLOR_BUFFER;

  //if (mIsMSAA2D)
  //    GX2CalcColorBufferAuxInfo(&mInnerBuffer, &mAuxBufferSize, &mAuxBufferAlign);
#endif // RIO_IS_CAFE
}

RenderTargetColor::RenderTargetColor()
    : RenderTarget<RenderTargetColor>()
  //, mIsMSAA2D(false)
  //, mAuxBufferSize(0)
  //, mAuxBufferAlign(0x800)
  //, mpAuxBuffer(nullptr)
{
#if RIO_IS_CAFE
    rio::MemUtil::set(&mInnerBuffer, 0, sizeof(GX2ColorBuffer));
#endif // RIO_IS_CAFE
}

void RenderTargetColor::invalidateGPUCache() const
{
#if RIO_IS_CAFE
    if (mTextureData.getImagePtr())
        GX2Invalidate(GX2_INVALIDATE_MODE_COLOR_BUFFER, mTextureData.getImagePtr(), mTextureData.getImageByteSize());

    if (mTextureData.getMipPtr())
        GX2Invalidate(GX2_INVALIDATE_MODE_COLOR_BUFFER, mTextureData.getMipPtr(), mTextureData.getMipByteSize());
#endif // RIO_IS_CAFE
}

void RenderTargetColor::initRegs_() const
{
    RIO_ASSERT(mMipLevel < mTextureData.getMipLevelNum());
#if RIO_IS_CAFE
    mInnerBuffer.viewMip = mMipLevel;
    mInnerBuffer.viewFirstSlice = 0;
    mInnerBuffer.viewNumSlices = 1;
    GX2InitColorBufferRegs(&mInnerBuffer);
#endif // RIO_IS_CAFE
    mUpdateRegs = false;
}

void RenderTargetDepth::onApplyTextureData_()
{
#if RIO_IS_CAFE
    mInnerBuffer.surface = mTextureData.getSurface();
    mInnerBuffer.surface.use = GX2_SURFACE_USE_DEPTH_BUFFER;

  //GX2CalcDepthBufferHiZInfo(&mInnerBuffer, &mHiZBufferSize, &mHiZBufferAlign);
#endif // RIO_IS_CAFE
}

RenderTargetDepth::RenderTargetDepth()
    : RenderTarget<RenderTargetDepth>()
  //, mHiZBufferSize(0)
  //, mHiZBufferAlign(0x800)
  //, mpHiZBuffer(nullptr)
{
#if RIO_IS_CAFE
    rio::MemUtil::set(&mInnerBuffer, 0, sizeof(GX2DepthBuffer));
    GX2InitDepthBufferHiZEnable(&mInnerBuffer, GX2_DISABLE);
#endif // RIO_IS_CAFE
}

void RenderTargetDepth::initRegs_() const
{
    RIO_ASSERT(mMipLevel < mTextureData.getMipLevelNum());
#if RIO_IS_CAFE
    mInnerBuffer.viewMip = mMipLevel;
    mInnerBuffer.viewFirstSlice = 0;
    mInnerBuffer.viewNumSlices = 1;
    mInnerBuffer.depthClear = 1.0f;
    mInnerBuffer.stencilClear = 0;
    GX2InitDepthBufferRegs(&mInnerBuffer);
#endif // RIO_IS_CAFE
    mUpdateRegs = false;
}

}
