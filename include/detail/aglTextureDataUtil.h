#pragma once

#include <common/aglTextureEnum.h>
#include <gpu/rio_Texture.h>

#include <cafe/gx2/gx2Surface.h>

namespace agl {

class TextureData;

namespace detail {

class TextureDataUtil
{
public:
    static TextureFormat convFormatGX2ToAGL(GX2SurfaceFormat format, bool color_target, bool depth_target);
    static GX2SurfaceFormat convFormatAGLToGX2(TextureFormat format);

    static void calcSizeAndAlignment(rio::NativeSurface2D* p_surface);
    static void initializeFromSurface(TextureData* p_texture_data, const GX2Surface& surface);

  //static void printInfo(const GX2Surface& surface);
};

} }
