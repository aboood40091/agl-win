#include <common/aglTextureData.h>
#include <common/aglTextureFormatInfo.h>
#include <detail/aglTextureDataUtil.h>
#include <misc/rio_MemUtil.h>

#if RIO_IS_CAFE
#include <gx2/mem.h>
#endif // RIO_IS_CAFE

namespace agl {

TextureData::TextureData()
    : mMinWidth(1)
    , mMinHeight(1)
    , mMaxMipLevel(1)
    , mCompR(cTextureCompSel_0)
    , mCompG(cTextureCompSel_0)
    , mCompB(cTextureCompSel_0)
    , mCompA(cTextureCompSel_0)
{
    rio::MemUtil::set(&mSurface, 0, sizeof(rio::NativeSurface2D));
}

void TextureData::setMipLevelNum(u32 mip_level_num)
{
    mSurface.mipLevels = std::clamp<s32>(mip_level_num, 1, mMaxMipLevel);
    detail::TextureDataUtil::calcSizeAndAlignment(&mSurface);

#if RIO_IS_WIN

    if (mHandle)
    {
        mHandle->bind();
        RIO_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mSurface.mipLevels - 1));
    }

#endif // RIO_IS_WIN
}

void TextureData::invalidateGPUCache() const
{
#if RIO_IS_CAFE
    if (getImagePtr())
        GX2Invalidate(GX2_INVALIDATE_MODE_TEXTURE, getImagePtr(), getImageByteSize());

    if (getMipPtr())
        GX2Invalidate(GX2_INVALIDATE_MODE_TEXTURE, getMipPtr(), getMipByteSize());
#endif
}

void TextureData::initialize(TextureFormat format, u32 width, u32 height, u32 mip_level_num)
{
#if RIO_IS_WIN
    RIO_ASSERT(!mHandle);
  //RIO_ASSERT(format != cTextureFormat_Depth_24_uNorm_Stencil_8);
#endif // RIO_IS_WIN

    mFormat = format;
    GX2SurfaceFormat surface_format = detail::TextureDataUtil::convFormatAGLToGX2(format);

#if RIO_IS_CAFE

    mSurface.format = surface_format;

    mSurface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
    mSurface.aa = GX2_AA_MODE1X;

    if (format >= cTextureFormat_Depth_16 && format <= cTextureFormat_Depth_24_uNorm_Stencil_8)
        mSurface.use = GX2_SURFACE_USE_TEXTURE | GX2_SURFACE_USE_DEPTH_BUFFER;
    else
        mSurface.use = GX2_SURFACE_USE_TEXTURE;

    mSurface.tileMode = GX2_TILE_MODE_DEFAULT;
    mSurface.swizzle = 0;

#elif RIO_IS_WIN

    mSurface.format = static_cast<rio::TextureFormat>(surface_format);

    [[maybe_unused]] bool success = TextureFormatInfo::setNativeTextureFormat(&mSurface.nativeFormat, mFormat);
    RIO_ASSERT(success);

#endif

    initializeSize_(width, height);
    setMipLevelNum(mip_level_num);

    mCompR = TextureFormatInfo::getDefaultCompSel(format, 0);
    mCompG = TextureFormatInfo::getDefaultCompSel(format, 1);
    mCompB = TextureFormatInfo::getDefaultCompSel(format, 2);
    mCompA = TextureFormatInfo::getDefaultCompSel(format, 3);

    detail::TextureDataUtil::calcSizeAndAlignment(&mSurface);

    mSurface.image = nullptr;
    mSurface.mipmaps = nullptr;
}

void TextureData::initializeSize_(u32 width, u32 height)
{
    mSurface.width = width;
    mSurface.height = height;
#if RIO_IS_CAFE
    mSurface.depth = 1;
#endif // RIO_IS_CAFE

    mMinWidth = 1;
    mMinHeight = 1;

    for (s32 mip_level = 1; ; mip_level++)
    {
        s32 mip_level_prev = mip_level - 1;
        if (getWidth(mip_level) == getWidth(mip_level_prev) &&
            getHeight(mip_level) == getHeight(mip_level_prev))
        {
            mMaxMipLevel = mip_level;
            return;
        }
    }
}

void TextureData::initializeFromSurface(const GX2Surface& surface)
{
    mFormat = detail::TextureDataUtil::convFormatGX2ToAGL(surface.format, surface.use & GX2_SURFACE_USE_COLOR_BUFFER, surface.use & GX2_SURFACE_USE_DEPTH_BUFFER);

    RIO_ASSERT(surface.dim == GX2_SURFACE_DIM_2D);
    RIO_ASSERT(surface.depth == 1);
    RIO_ASSERT(surface.aa == GX2_AA_MODE_1X);
    RIO_ASSERT(surface.use & GX2_SURFACE_USE_TEXTURE);
    RIO_ASSERT(surface.imageSize && surface.imagePtr);
    RIO_ASSERT((surface.numMips <= 1 && !surface.mipSize) ||
               (surface.numMips > 1 && surface.mipSize && surface.mipPtr));

#if RIO_IS_CAFE

    mSurface = surface;

#elif RIO_IS_WIN

    RIO_ASSERT(!mHandle);
    RIO_ASSERT(surface.tileMode == GX2_TILE_MODE_LINEAR_SPECIAL);

    mSurface.width = surface.width;
    mSurface.height = surface.height;
    mSurface.mipLevels = surface.numMips;
    mSurface.format = rio::TextureFormat(surface.format);

    [[maybe_unused]] bool success = TextureFormatInfo::setNativeTextureFormat(&mSurface.nativeFormat, mFormat);
    RIO_ASSERT(success);

    mSurface.imageSize = surface.imageSize;
    mSurface.mipmapSize = surface.mipSize;
    mSurface.mipLevelOffset[0] = 0;
    rio::MemUtil::copy(&mSurface.mipLevelOffset[1], &surface.mipOffset[1], sizeof(u32) * (13 - 1));
    mSurface.image = surface.imagePtr;
    mSurface.mipmaps = surface.mipPtr;

    mHandle = std::make_shared<TextureHandle>();
    mHandle->bind();

    RIO_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
    RIO_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mSurface.mipLevels - 1));

    RIO_GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    switch (mSurface.format)
    {
    case rio::TEXTURE_FORMAT_BC1_UNORM:
    case rio::TEXTURE_FORMAT_BC2_UNORM:
    case rio::TEXTURE_FORMAT_BC3_UNORM:
    case rio::TEXTURE_FORMAT_BC4_UNORM:
    case rio::TEXTURE_FORMAT_BC4_SNORM:
    case rio::TEXTURE_FORMAT_BC5_UNORM:
    case rio::TEXTURE_FORMAT_BC5_SNORM:
        {
            RIO_GL_CALL(glCompressedTexImage2D(
                GL_TEXTURE_2D,
                0,
                mSurface.nativeFormat.internalformat,
                mSurface.width,
                mSurface.height,
                0,
                mSurface.imageSize,
                mSurface.image
            ));

            if (mSurface.mipLevels > 1)
            {
                for (u32 i = 0; i < mSurface.mipLevels - 2; i++)
                    RIO_GL_CALL(glCompressedTexImage2D(
                        GL_TEXTURE_2D,
                        i + 1,
                        mSurface.nativeFormat.internalformat,
                        std::max(mSurface.width  >> (i + 1), 1u),
                        std::max(mSurface.height >> (i + 1), 1u),
                        0,
                        mSurface.mipLevelOffset[i + 1] - mSurface.mipLevelOffset[i],
                        (u8*)mSurface.mipmaps + mSurface.mipLevelOffset[i]
                    ));

                RIO_GL_CALL(glCompressedTexImage2D(
                    GL_TEXTURE_2D,
                    (mSurface.mipLevels - 2) + 1,
                    mSurface.nativeFormat.internalformat,
                    std::max(mSurface.width  >> ((mSurface.mipLevels - 2) + 1), 1u),
                    std::max(mSurface.height >> ((mSurface.mipLevels - 2) + 1), 1u),
                    0,
                    mSurface.mipmapSize - mSurface.mipLevelOffset[(mSurface.mipLevels - 2)],
                    (u8*)mSurface.mipmaps + mSurface.mipLevelOffset[(mSurface.mipLevels - 2)]
                ));
            }
        }
        break;
    case rio::TEXTURE_FORMAT_BC1_SRGB:
    case rio::TEXTURE_FORMAT_BC2_SRGB:
    case rio::TEXTURE_FORMAT_BC3_SRGB:
        {
            RIO_GL_CALL(glCompressedTexImage2DARB(
                GL_TEXTURE_2D,
                0,
                mSurface.nativeFormat.internalformat,
                mSurface.width,
                mSurface.height,
                0,
                mSurface.imageSize,
                mSurface.image
            ));

            if (mSurface.mipLevels > 1)
            {
                for (u32 i = 0; i < mSurface.mipLevels - 2; i++)
                    RIO_GL_CALL(glCompressedTexImage2DARB(
                        GL_TEXTURE_2D,
                        i + 1,
                        mSurface.nativeFormat.internalformat,
                        std::max(mSurface.width  >> (i + 1), 1u),
                        std::max(mSurface.height >> (i + 1), 1u),
                        0,
                        mSurface.mipLevelOffset[i + 1] - mSurface.mipLevelOffset[i],
                        (u8*)mSurface.mipmaps + mSurface.mipLevelOffset[i]
                    ));

                RIO_GL_CALL(glCompressedTexImage2DARB(
                    GL_TEXTURE_2D,
                    (mSurface.mipLevels - 2) + 1,
                    mSurface.nativeFormat.internalformat,
                    std::max(mSurface.width  >> ((mSurface.mipLevels - 2) + 1), 1u),
                    std::max(mSurface.height >> ((mSurface.mipLevels - 2) + 1), 1u),
                    0,
                    mSurface.mipmapSize - mSurface.mipLevelOffset[(mSurface.mipLevels - 2)],
                    (u8*)mSurface.mipmaps + mSurface.mipLevelOffset[(mSurface.mipLevels - 2)]
                ));
            }
        }
        break;
    default:
        {
            RIO_GL_CALL(glTexImage2D(
                GL_TEXTURE_2D,
                0,
                mSurface.nativeFormat.internalformat,
                mSurface.width,
                mSurface.height,
                0,
                mSurface.nativeFormat.format,
                mSurface.nativeFormat.type,
                mSurface.image
            ));

            if (mSurface.mipLevels > 1)
            {
                for (u32 i = 0; i < mSurface.mipLevels - 2; i++)
                    RIO_GL_CALL(glTexImage2D(
                        GL_TEXTURE_2D,
                        i + 1,
                        mSurface.nativeFormat.internalformat,
                        std::max(mSurface.width  >> (i + 1), 1u),
                        std::max(mSurface.height >> (i + 1), 1u),
                        0,
                        mSurface.nativeFormat.format,
                        mSurface.nativeFormat.type,
                        (u8*)mSurface.mipmaps + mSurface.mipLevelOffset[i]
                    ));

                RIO_GL_CALL(glTexImage2D(
                    GL_TEXTURE_2D,
                    (mSurface.mipLevels - 2) + 1,
                    mSurface.nativeFormat.internalformat,
                    std::max(mSurface.width  >> ((mSurface.mipLevels - 2) + 1), 1u),
                    std::max(mSurface.height >> ((mSurface.mipLevels - 2) + 1), 1u),
                    0,
                    mSurface.nativeFormat.format,
                    mSurface.nativeFormat.type,
                    (u8*)mSurface.mipmaps + mSurface.mipLevelOffset[(mSurface.mipLevels - 2)]
                ));
            }
        }
        break;
    }

#endif

    initializeSize_(mSurface.width, mSurface.height);
    setMipLevelNum(surface.numMips);

    mCompR = TextureFormatInfo::getDefaultCompSel(mFormat, 0);
    mCompG = TextureFormatInfo::getDefaultCompSel(mFormat, 1);
    mCompB = TextureFormatInfo::getDefaultCompSel(mFormat, 2);
    mCompA = TextureFormatInfo::getDefaultCompSel(mFormat, 3);
}

}
