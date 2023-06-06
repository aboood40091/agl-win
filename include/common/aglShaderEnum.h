#pragma once

#include <misc/rio_Types.h>

namespace agl {

// No idea which headers these are actually supposed to go in

enum ShaderType : s32
{
    cShaderType_Vertex,
    cShaderType_Fragment,
    cShaderType_Geometry,
    cShaderType_Num
};

enum ShaderSymbolType
{
    cShaderSymbolType_Uniform,
    cShaderSymbolType_UniformBlock,
    cShaderSymbolType_Sampler,
    cShaderSymbolType_Attribute,
    cShaderSymbolType_Num,
};

enum ShaderMode
{
    cShaderMode_UniformRegister,
    cShaderMode_UniformBlock,
    cShaderMode_GeometryShader,
    cShaderMode_Invalid
};

}
