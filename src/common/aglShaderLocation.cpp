#include <common/aglShaderLocation.h>
#include <common/aglShaderProgram.h>

#if RIO_IS_CAFE

namespace {

static inline GX2SamplerVar*
GX2GetVertexSamplerVar(const GX2VertexShader* shader,
                       const char* name)
{
    for (u32 i = 0; i < shader->samplerVarCount; i++)
    {
       if (strcmp(shader->samplerVars[i].name, name) == 0)
           return &shader->samplerVars[i];
    }

    return nullptr;
}

static inline GX2SamplerVar*
GX2GetGeometrySamplerVar(const GX2GeometryShader* shader,
                         const char* name)
{
    for (u32 i = 0; i < shader->samplerVarCount; i++)
    {
       if (strcmp(shader->samplerVars[i].name, name) == 0)
           return &shader->samplerVars[i];
    }

    return nullptr;
}

static inline s32
GX2GetVertexUniformVarOffset(const GX2VertexShader* shader,
                             const char* name)
{
    GX2UniformVar* uniform_var = GX2GetVertexUniformVar(shader, name);
    if (!uniform_var)
        return -1;

    return (s32)uniform_var->offset;
}

static inline s32
GX2GetPixelUniformVarOffset(const GX2PixelShader* shader,
                            const char* name)
{
    GX2UniformVar* uniform_var = GX2GetPixelUniformVar(shader, name);
    if (!uniform_var)
        return -1;

    return (s32)uniform_var->offset;
}

static inline s32
GX2GetGeometryUniformVarOffset(const GX2GeometryShader* shader,
                               const char* name)
{
    GX2UniformVar* uniform_var = GX2GetGeometryUniformVar(shader, name);
    if (!uniform_var)
        return -1;

    return (s32)uniform_var->offset;
}

}

#elif RIO_IS_WIN

#include <sstream>

namespace {

static inline s32
GetVertexAttribLocation(const rio::Shader& shader, const GX2VertexShader* p_shader, const char* name)
{
    for (u32 i = 0; i < p_shader->numAttribs; i++)
    {
        if (std::strcmp(p_shader->attribVars[i].name, name) == 0)
        {
            std::ostringstream attribNameStrm;
            attribNameStrm << name << "_0_0";
            return shader.getVertexAttribLocation(attribNameStrm.str().c_str());
        }
    }

    return -1;
}

}

#endif

namespace agl {

void UniformLocation::search(const ShaderProgram& program)
{
#if RIO_IS_CAFE
    if (program.getVertexShaderBinary())
        mVS = GX2GetVertexUniformVarOffset(program.getVertexShaderBinary(), getName());
    else
        mVS = -1;

    if (program.getFragmentShaderBinary())
        mFS = GX2GetPixelUniformVarOffset(program.getFragmentShaderBinary(), getName());
    else
        mFS = -1;

    if (program.getGeometryShaderBinary())
        mGS = GX2GetGeometryUniformVarOffset(program.getGeometryShaderBinary(), getName());
    else
        mGS = -1;
#else
#if RIO_IS_WIN
    RIO_LOG("Trying to uniform (%s) location from shader program (%s)\n", getName(), program.getName());
#endif // RIO_IS_WIN
    mVS = -1;
    mFS = -1;
    mGS = -1;
#endif
}

void UniformBlockLocation::search(const ShaderProgram& program)
{
    mVS = -1;
    mFS = -1;
    mGS = -1;

#if RIO_IS_CAFE
    const GX2VertexShader* vertex_shader = program.getVertexShaderBinary();
    if (vertex_shader)
    {
        GX2UniformBlock* uniform_block = GX2GetVertexUniformBlock(vertex_shader, getName());
        if (uniform_block)
            mVS = uniform_block->offset;
    }
    const GX2PixelShader* pixel_shader = program.getFragmentShaderBinary();
    if (pixel_shader)
    {
        GX2UniformBlock* uniform_block = GX2GetPixelUniformBlock(pixel_shader, getName());
        if (uniform_block)
            mFS = uniform_block->offset;
    }
    const GX2GeometryShader* geometry_shader = program.getGeometryShaderBinary();
    if (geometry_shader)
    {
        GX2UniformBlock* uniform_block = GX2GetGeometryUniformBlock(geometry_shader, getName());
        if (uniform_block)
            mGS = uniform_block->offset;
    }
#elif RIO_IS_WIN
  //RIO_LOG("Getting uniform block (%s) location from shader program (%s)\n", getName(), program.getName());
    mVS = program.getShaderRIO().getVertexUniformBlockIndex(getName());
    mFS = program.getShaderRIO().getFragmentUniformBlockIndex(getName());
    mGS = -1;
#endif // cafe
}

void SamplerLocation::search(const ShaderProgram& program)
{
    mVS = -1;
    mFS = -1;
    mGS = -1;

#if RIO_IS_CAFE
    const GX2VertexShader* vertex_shader = program.getVertexShaderBinary();
    if (vertex_shader)
    {
        GX2SamplerVar* sampler_var = GX2GetVertexSamplerVar(vertex_shader, getName());
        if (sampler_var)
            mVS = sampler_var->location;
    }
    const GX2PixelShader* pixel_shader = program.getFragmentShaderBinary();
    if (pixel_shader)
    {
        GX2SamplerVar* sampler_var = GX2GetPixelSamplerVar(pixel_shader, getName());
        if (sampler_var)
            mFS = sampler_var->location;
    }
    const GX2GeometryShader* geometry_shader = program.getGeometryShaderBinary();
    if (geometry_shader)
    {
        GX2SamplerVar* sampler_var = GX2GetGeometrySamplerVar(geometry_shader, getName());
        if (sampler_var)
            mGS = sampler_var->location;
    }
#elif RIO_IS_WIN
  //RIO_LOG("Getting sampler (%s) location from shader program (%s)\n", getName(), program.getName());
    mVS = program.getShaderRIO().getVertexSamplerLocation(getName());
    mFS = program.getShaderRIO().getFragmentSamplerLocation(getName());
    mGS = -1;
#endif // cafe
}

void AttributeLocation::search(const ShaderProgram& program)
{
#if RIO_IS_CAFE
    // if (program.getVertexShaderBinary())
        mVS = GX2GetVertexAttribVarLocation(program.getVertexShaderBinary(), getName());
    // else
    //     mVS = -1;
#elif RIO_IS_WIN
  //RIO_LOG("Getting attribute (%s) location from shader program (%s)\n", getName(), program.getName());
    mVS = GetVertexAttribLocation(program.getShaderRIO(), program.getVertexShaderBinary(), getName());
#else
    mVS = -1;
#endif
}

}
