#pragma once

#include <common/aglTextureSampler.h>
#include <container/SafeArray.h>

namespace agl { namespace utl {

class PrimitiveTexture
{
public:
    static bool createSingleton();
    static void destroySingleton();
    static PrimitiveTexture* instance() { return sInstance; }

private:
    static PrimitiveTexture* sInstance;

    PrimitiveTexture(const PrimitiveTexture&);
    PrimitiveTexture& operator=(const PrimitiveTexture&);

public:
    enum TextureSamplerType
    {
        cSampler_White2D = 0,
        cSampler_Gray2D,
        cSampler_Black2D,
      //cSampler_Black2DArray,
      //cSampler_BlackCube,
      //cSampler_BlackCubeArray,
        cSampler_Depth32_0,
        cSampler_Depth32_1,
      //cSampler_MipLevel,
        cSampler_DepthShadow,
      //cSampler_DepthShadowArray,
        cSampler_Num
    };
  //static_assert(cSampler_Num == 11);

public:
    PrimitiveTexture();
    virtual ~PrimitiveTexture();

    void initialize();

/*
private:
    void initializeMipLevelTexture_();
*/

public:
  //TextureSampler& getTextureSampler(TextureSamplerType sampler) { return mTextureSampler[sampler]; }
    const TextureSampler& getTextureSampler(TextureSamplerType sampler) const { return mTextureSampler[sampler]; }

private:
    SafeArray<TextureSampler, cSampler_Num> mTextureSampler;
};
//static_assert(sizeof(PrimitiveTexture) == 0x11F4, "agl::utl::PrimitiveTexture size mismatch");


} }
