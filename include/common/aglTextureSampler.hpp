#pragma once

namespace agl {

inline void
TextureSampler::applyTextureData(const TextureData& texture_data)
{
    if (mTextureData != texture_data)
        applyTextureData_(texture_data);

    else
    {
        mTextureData.shareMemory(texture_data);

#if RIO_IS_CAFE
        void* image_ptr = texture_data.getImagePtr();
        void* mip_ptr = texture_data.getMipPtr();

        mGX2Texture.surface.imagePtr = image_ptr;
        mGX2Texture.surface.mipPtr   = mip_ptr;
#endif // RIO_IS_CAFE

#if RIO_IS_CAFE

    mGX2Texture.compSel = GX2_GET_COMP_SEL(mTextureData.getComponentR(),
                                           mTextureData.getComponentG(),
                                           mTextureData.getComponentB(),
                                           mTextureData.getComponentA());
    GX2InitTextureRegs(&mGX2Texture);

    mSamplerInner.linkNativeTexture2D(&mGX2Texture);

#elif RIO_IS_WIN

    const std::shared_ptr<TextureHandle>& handle = mTextureData.getHandle();
    RIO_ASSERT(handle);
    handle->bind();

    static const GLint TEX_COMP_MAP_TO_GL[6] = {
        GL_RED, GL_GREEN, GL_BLUE,
        GL_ALPHA, GL_ZERO, GL_ONE
    };

    GLint compMap[4] = {
        TEX_COMP_MAP_TO_GL[mTextureData.getComponentR()],
        TEX_COMP_MAP_TO_GL[mTextureData.getComponentG()],
        TEX_COMP_MAP_TO_GL[mTextureData.getComponentB()],
        TEX_COMP_MAP_TO_GL[mTextureData.getComponentA()]
    };

    RIO_GL_CALL(glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, compMap));

    mSamplerInner.linkNativeTexture2D(handle->mHandle);

#endif
    }
}

inline void
TextureSampler::setWrapX(TextureWrapType wrap_x)
{
    mSamplerInner.setWrapX((rio::TexWrapMode)wrap_x);
}

inline void
TextureSampler::setWrapY(TextureWrapType wrap_y)
{
    mSamplerInner.setWrapY((rio::TexWrapMode)wrap_y);
}

inline void
TextureSampler::setWrapZ(TextureWrapType wrap_z)
{
    mSamplerInner.setWrapZ((rio::TexWrapMode)wrap_z);
}

inline void
TextureSampler::setWrap(
    TextureWrapType wrap_x,
    TextureWrapType wrap_y,
    TextureWrapType wrap_z
)
{
    mSamplerInner.setWrap(
        (rio::TexWrapMode)wrap_x,
        (rio::TexWrapMode)wrap_y,
        (rio::TexWrapMode)wrap_z
    );
}

inline void
TextureSampler::setFilterMag(TextureFilterType filter_mag)
{
    mSamplerInner.setMagFilter((rio::TexXYFilterMode)filter_mag);
}

inline void
TextureSampler::setFilterMin(TextureFilterType filter_min)
{
    mSamplerInner.setMinFilter((rio::TexXYFilterMode)filter_min);
}

inline void
TextureSampler::setFilterMip(TextureMipFilterType filter_mip)
{
    mSamplerInner.setMipFilter((rio::TexMipFilterMode)filter_mip);
}

inline void
TextureSampler::setFilter(
    TextureFilterType    filter_mag,
    TextureFilterType    filter_min,
    TextureMipFilterType filter_mip
)
{
    mSamplerInner.setMagFilter((rio::TexXYFilterMode )filter_mag);
    mSamplerInner.setMinFilter((rio::TexXYFilterMode )filter_min);
    mSamplerInner.setMipFilter((rio::TexMipFilterMode)filter_mip);
}

inline void
TextureSampler::setMaxAnisoRatio(TextureAnisoRatio max_aniso)
{
    mSamplerInner.setMaxAnisoRatio((rio::TexAnisoRatio)max_aniso);
}

inline void
TextureSampler::setMipParam(f32 lod_min, f32 lod_max, f32 lod_bias)
{
    mSamplerInner.setLOD(lod_min, lod_max, lod_bias);
}

inline void
TextureSampler::setBorderColor(f32 r, f32 g, f32 b, f32 a)
{
    mSamplerInner.setBorderColor(r, g, b, a);
}

inline void
TextureSampler::setBorderColor(const rio::Color4f& color)
{
    setBorderColor(color.r, color.g, color.b, color.a);
}

}
