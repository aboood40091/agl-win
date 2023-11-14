#include <common/aglTextureSampler.h>
#include <detail/aglShaderHolder.h>
#include <math/rio_Matrix.h>
#include <utility/aglDevTools.h>
#include <utility/aglImageFilter2D.h>
#include <utility/aglPrimitiveShape.h>
#include <utility/aglVertexAttributeHolder.h>

namespace agl { namespace utl {

void DevTools::drawTexture_(const ShaderProgram& program, const TextureSampler& sampler, const rio::BaseMtx34f& world_view_mtx, const rio::BaseMtx44f& proj_mtx, const rio::Color4f& color)
{
    sampler.activate(program.getSamplerLocationValidate(ImageFilter2D::cSampler_Texture), ImageFilter2D::cSampler_Texture);

    rio::Matrix44f wvp_mtx;
    wvp_mtx.setMul(static_cast<const rio::Matrix44f&>(proj_mtx), static_cast<const rio::Matrix34f&>(world_view_mtx));

    program.getUniformLocation(ImageFilter2D::cUniform_ProjViewWorld).setVec4Array(wvp_mtx);

    program.getUniformLocation(ImageFilter2D::cUniform_Color).setVec4(color);

    VertexAttributeHolder::instance()->getVertexAttribute(VertexAttributeHolder::cAttribute_Quad).activate();
    PrimitiveShape::instance()->getIdxStreamQuad().draw();
}

ShaderMode DevTools::drawTextureTexCoord(const TextureSampler& sampler, const rio::BaseMtx34f& world_view_mtx, const rio::BaseMtx44f& proj_mtx, const rio::BaseVec2f& tex_scale, f32 tex_rotate, const rio::BaseVec2f& tex_coord, ShaderMode mode)
{
    const ShaderProgram& program =
        detail::ShaderHolder::instance()->getShader(detail::ShaderHolder::cShader_TextureTexCoord);

    mode = program.activate(mode);

    program.getUniformLocation(ImageFilter2D::cUniform_TexScale).setVec2(tex_scale);
    program.getUniformLocation(ImageFilter2D::cUniform_TexRotate).setFloat(tex_rotate);
    program.getUniformLocation(ImageFilter2D::cUniform_TexTrans).setVec2(tex_coord);

    drawTexture_(program, sampler, world_view_mtx, proj_mtx, rio::Color4f::cWhite);

    return mode;
}


} }
