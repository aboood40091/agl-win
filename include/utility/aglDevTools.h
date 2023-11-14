#pragma once

#include <common/aglShaderEnum.h>
#include <gfx/rio_Color.h>
#include <math/rio_MathTypes.h>

namespace agl {

class ShaderProgram;
class TextureSampler;

namespace utl {

class DevTools
{
private:
    static void drawTexture_(const ShaderProgram& program, const TextureSampler& sampler, const rio::BaseMtx34f& world_view_mtx, const rio::BaseMtx44f& proj_mtx, const rio::Color4f& color);

public:
    static ShaderMode drawTextureTexCoord(const TextureSampler& sampler, const rio::BaseMtx34f& world_view_mtx, const rio::BaseMtx44f& proj_mtx, const rio::BaseVec2f& tex_scale, f32 tex_rotate, const rio::BaseVec2f& tex_coord, ShaderMode mode);
};

} }
