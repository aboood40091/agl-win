#pragma once

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
