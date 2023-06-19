#pragma once

#include <common/aglTextureData.h>

namespace agl {

template <typename T>
class RenderTarget
{
public:
    static const u32 cMipLevelMax = 14;

public:
    RenderTarget()
        : mTextureData()
        , mUpdateRegs(true)
        , mMipLevel(0)
    {
    }

    TextureData& getTextureData() { return mTextureData; }
    const TextureData& getTextureData() const { return mTextureData; }

    u32 getMipLevel() const { return mMipLevel; }

    void setMipLevel(u32 mip_level)
    {
        RIO_ASSERT(mip_level < cMipLevelMax);
        if (mMipLevel != mip_level)
        {
            mMipLevel = mip_level;
            mUpdateRegs = true;
        }
    }

    void applyTextureData(const TextureData& texture_data);
    void applyTextureData(const TextureData& texture_data, u32 mip_level);

protected:
    void applyTextureData_(const TextureData& texture_data, u32 mip_level);

protected:
    TextureData mTextureData;
    mutable bool mUpdateRegs;
    u32 mMipLevel;
};
//static_assert(sizeof(RenderTarget<void>) == 0xA8);

class RenderTargetColor : public RenderTarget<RenderTargetColor>
{
    void onApplyTextureData_();
    friend class RenderTarget<RenderTargetColor>;

public:
    RenderTargetColor();
    RenderTargetColor(const TextureData& texture_data, u32 mip_level);

    void invalidateGPUCache() const;

    void bind() const;

private:
    void initRegs_() const;

protected:
#if RIO_IS_CAFE
    mutable GX2ColorBuffer mInnerBuffer;
#endif // RIO_IS_CAFE
};
//static_assert(sizeof(RenderTargetColor) == 0x154, "agl::RenderTargetColor size mismatch");

class RenderTargetDepth : public RenderTarget<RenderTargetDepth>
{
    void onApplyTextureData_();
    friend class RenderTarget<RenderTargetDepth>;

public:
    RenderTargetDepth();
    RenderTargetDepth(const TextureData& texture_data, u32 mip_level);

    void invalidateGPUCache() const;

    void bind() const;

private:
    void initRegs_() const;

protected:
#if RIO_IS_CAFE
    mutable GX2DepthBuffer mInnerBuffer;
#endif // RIO_IS_CAFE
};
//static_assert(sizeof(RenderTargetDepth) == 0x160, "agl::RenderTargetDepth size mismatch");

}

#ifdef __cplusplus

#include <common/aglRenderTarget.hpp>

#endif // __cplusplus
