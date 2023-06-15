#pragma once

#include <math/rio_MathTypes.h>
#include <misc/Namable.h>

namespace agl {

class ShaderLocation
{
public:
    ShaderLocation()
        : mVS(-1)
        , mFS(-1)
        , mGS(-1)
    {
    }

    bool isValid() const
    {
        return getVertexLocation()   != -1 ||
               getFragmentLocation() != -1 ||
               getGeometryLocation() != -1;
    }

    s32 getVertexLocation() const { return mVS; }
    s32 getFragmentLocation() const { return mFS; }
    s32 getGeometryLocation() const { return mGS; }

protected:
    s16 mVS;
    s16 mFS;
    s16 mGS;
};
static_assert(sizeof(ShaderLocation) == 6, "agl::ShaderLocation size mismatch");

class ShaderProgram;

class UniformLocation : public INamable, public ShaderLocation
{
public:
    UniformLocation()
        : INamable("Undefined")
    {
    }

    explicit UniformLocation(const char* name)
        : INamable(name)
    {
    }

    UniformLocation(const char* name, const ShaderProgram& program)
        : INamable(name)
    {
        search(program);
    }

    UniformLocation& operator=(const UniformLocation& rhs)
    {
        setName(rhs.getName());
        ShaderLocation::operator=(static_cast<const ShaderLocation&>(rhs));
        return *this;
    }

    void search(const ShaderProgram& program);

    // GLSL type: float
    void setUniform(f32 v) const;

    // GLSL type: int
    void setUniform(s32 v) const;

    // GLSL type: uint
    void setUniform(u32 v) const;

    // GLSL type: vec2
    void setUniform(const rio::BaseVec2f& v) const;
    void setUniform(f32 v0, f32 v1) const
    {
        setUniform(rio::BaseVec2f{ v0, v1 });
    }

    // GLSL type: ivec2
    void setUniform(const rio::BaseVec2i& v) const;
    void setUniform(s32 v0, s32 v1) const
    {
        setUniform(rio::BaseVec2i{ v0, v1 });
    }

    // GLSL type: uvec2
    void setUniform(const rio::BaseVec2u& v) const;
    void setUniform(u32 v0, u32 v1) const
    {
        setUniform(rio::BaseVec2u{ v0, v1 });
    }

    // GLSL type: vec3
    void setUniform(const rio::BaseVec3f& v) const;
    void setUniform(f32 v0, f32 v1, f32 v2) const
    {
        setUniform(rio::BaseVec3f{ v0, v1, v2 });
    }

    // GLSL type: ivec3
    void setUniform(const rio::BaseVec3i& v) const;
    void setUniform(s32 v0, s32 v1, s32 v2) const
    {
        setUniform(rio::BaseVec3i{ v0, v1, v2 });
    }

    // GLSL type: uvec3
    void setUniform(const rio::BaseVec3u& v) const;
    void setUniform(u32 v0, u32 v1, u32 v2) const
    {
        setUniform(rio::BaseVec3u{ v0, v1, v2 });
    }

    // GLSL type: vec4
    void setUniform(const rio::BaseVec4f& v) const;
    void setUniform(f32 v0, f32 v1, f32 v2, f32 v3) const
    {
        setUniform(rio::BaseVec4f{ v0, v1, v2, v3 });
    }

    // GLSL type: ivec4
    void setUniform(const rio::BaseVec4i& v) const;
    void setUniform(s32 v0, s32 v1, s32 v2, s32 v3) const
    {
        setUniform(rio::BaseVec4i{ v0, v1, v2, v3 });
    }

    // GLSL type: uvec4
    void setUniform(const rio::BaseVec4u& v) const;
    void setUniform(u32 v0, u32 v1, u32 v2, u32 v3) const
    {
        setUniform(rio::BaseVec4u{ v0, v1, v2, v3 });
    }

    // GLSL type: mtx2
    void setUniform(const rio::BaseMtx22f& v) const;

    // GLSL type: mtx32
    void setUniform(const rio::BaseMtx23f& v) const;

    // GLSL type: mtx42
    void setUniform(const rio::BaseMtx24f& v) const;

    // GLSL type: mtx23
    void setUniform(const rio::BaseMtx32f& v) const;

    // GLSL type: mtx3
    void setUniform(const rio::BaseMtx33f& v) const;

    // GLSL type: mtx43
    void setUniform(const rio::BaseMtx34f& v) const;

    // GLSL type: mtx24
    void setUniform(const rio::BaseMtx42f& v) const;

    // GLSL type: mtx34
    void setUniform(const rio::BaseMtx43f& v) const;

    // GLSL type: mtx4
    void setUniform(const rio::BaseMtx44f& v) const;

    // GLSL type: float[]
    //void setUniformArray(u32 count, f32* v) const;

    // GLSL type: int[]
    //void setUniformArray(u32 count, s32* v) const;

    // GLSL type: uint[]
    //void setUniformArray(u32 count, u32* v) const;

    // GLSL type: vec2[]
    //void setUniformArray(u32 count, const rio::BaseVec2f* v) const;

    // GLSL type: ivec2[]
    //void setUniformArray(u32 count, const rio::BaseVec2i* v) const;

    // GLSL type: uvec2[]
    //void setUniformArray(u32 count, const rio::BaseVec2u* v) const;

    // GLSL type: vec3[]
    //void setUniformArray(u32 count, const rio::BaseVec3f* v) const;

    // GLSL type: ivec3[]
    //void setUniformArray(u32 count, const rio::BaseVec3i* v) const;

    // GLSL type: uvec3[]
    //void setUniformArray(u32 count, const rio::BaseVec3u* v) const;

    // GLSL type: vec4[]
    void setUniformArray(u32 count, const rio::BaseVec4f* v) const;

    // GLSL type: ivec4[]
    void setUniformArray(u32 count, const rio::BaseVec4i* v) const;

    // GLSL type: uvec4[]
    void setUniformArray(u32 count, const rio::BaseVec4u* v) const;

    // GLSL type: mtx2[]
    //void setUniformArray(u32 count, const rio::BaseMtx22f* v) const;

    // GLSL type: mtx32[]
    //void setUniformArray(u32 count, const rio::BaseMtx23f* v) const;

    // GLSL type: mtx42[]
    //void setUniformArray(u32 count, const rio::BaseMtx24f* v) const;

    // GLSL type: mtx23[]
    //void setUniformArray(u32 count, const rio::BaseMtx32f* v) const;

    // GLSL type: mtx3[]
    //void setUniformArray(u32 count, const rio::BaseMtx33f* v) const;

    // GLSL type: mtx43[]
    //void setUniformArray(u32 count, const rio::BaseMtx34f* v) const;

    // GLSL type: mtx24[]
    //void setUniformArray(u32 count, const rio::BaseMtx42f* v) const;

    // GLSL type: mtx34[]
    //void setUniformArray(u32 count, const rio::BaseMtx43f* v) const;

    // GLSL type: mtx4[]
    //void setUniformArray(u32 count, const rio::BaseMtx44f* v) const;
};
static_assert(sizeof(UniformLocation) == 0x10, "agl::UniformLocation size mismatch");

class UniformBlockLocation : public INamable, public ShaderLocation
{
public:
    UniformBlockLocation()
        : INamable("Undefined")
    {
    }

    explicit UniformBlockLocation(const char* name)
        : INamable(name)
    {
    }

    UniformBlockLocation(const char* name, const ShaderProgram& program)
        : INamable(name)
    {
        search(program);
    }

    UniformBlockLocation& operator=(const UniformBlockLocation& rhs)
    {
        setName(rhs.getName());
        ShaderLocation::operator=(static_cast<const ShaderLocation&>(rhs));
        return *this;
    }

    void search(const ShaderProgram& program);
};
static_assert(sizeof(UniformBlockLocation) == 0x10, "agl::UniformBlockLocation size mismatch");

class SamplerLocation : public INamable, public ShaderLocation
{
public:
    SamplerLocation()
        : INamable("Undefined")
    {
    }

    explicit SamplerLocation(const char* name)
        : INamable(name)
    {
    }

    SamplerLocation(const char* name, const ShaderProgram& program)
        : INamable(name)
    {
        search(program);
    }

    SamplerLocation& operator=(const SamplerLocation& rhs)
    {
        setName(rhs.getName());
        ShaderLocation::operator=(static_cast<const ShaderLocation&>(rhs));
        return *this;
    }

    void search(const ShaderProgram& program);
};
static_assert(sizeof(SamplerLocation) == 0x10, "agl::SamplerLocation size mismatch");

class AttributeLocation : public INamable
{
public:
    AttributeLocation()
        : INamable("Undefined")
    {
    }

    explicit AttributeLocation(const char* name)
        : INamable(name)
    {
    }

    AttributeLocation(const char* name, const ShaderProgram& program)
        : INamable(name)
    {
        search(program);
    }

    bool isValid() const
    {
        return getVertexLocation() != -1;
    }

    s32 getVertexLocation() const { return mVS; }

    void search(const ShaderProgram& program);

private:
    s16 mVS;
};
static_assert(sizeof(AttributeLocation) == 0xC, "agl::AttributeLocation size mismatch");

}

#ifdef __cplusplus

#include <common/aglShaderLocation.hpp>

#endif // __cplusplus
