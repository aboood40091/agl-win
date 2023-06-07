#pragma once

#if RIO_IS_CAFE
#include <cafe/gx2.h>
#elif RIO_IS_WIN
#include <misc/gl/rio_GL.h>
#endif

namespace agl {

inline
void UniformLocation::setUniform(f32 v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    setUniform(rio::BaseVec4f{ v, 0.0f, 0.0f, 0.0f });

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform1f(location, v));

#endif
}

inline
void UniformLocation::setUniform(s32 v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    setUniform(rio::BaseVec4i{ v, 0, 0, 0 });

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform1i(location, v));

#endif
}

inline
void UniformLocation::setUniform(u32 v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    setUniform(rio::BaseVec4u{ v, 0, 0, 0 });

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform1ui(location, v));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseVec2f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    setUniform(rio::BaseVec4f{ v.x, v.y, 0.0f, 0.0f });

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform2fv(location, 1, &v.x));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseVec2i& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    setUniform(rio::BaseVec4i{ v.x, v.y, 0, 0 });

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform2iv(location, 1, &v.x));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseVec2u& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    setUniform(rio::BaseVec4u{ v.x, v.y, 0, 0 });

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform2uiv(location, 1, &v.x));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseVec3f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    setUniform(rio::BaseVec4f{ v.x, v.y, v.z, 0.0f });

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform3fv(location, 1, &v.x));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseVec3i& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    setUniform(rio::BaseVec4i{ v.x, v.y, v.z, 0 });

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform3iv(location, 1, &v.x));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseVec3u& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    setUniform(rio::BaseVec4u{ v.x, v.y, v.z, 0 });

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform3uiv(location, 1, &v.x));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseVec4f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, 4, &v.x);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, 4, &v.x);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform4fv(location, 1, &v.x));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseVec4i& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, 4, &v.x);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, 4, &v.x);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform4iv(location, 1, &v.x));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseVec4u& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, 4, &v.x);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, 4, &v.x);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform4uiv(location, 1, &v.x));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseMtx22f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS == -1 && mFS == -1)
        return;

    const u32 NUM_COL = 2;

    f32 mat_trans[NUM_COL][4] = {
        { v.m[0][0], v.m[1][0] },
        { v.m[0][1], v.m[1][1] }
    };

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, NUM_COL*4, mat_trans);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, NUM_COL*4, mat_trans);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix2fv(location, 1, GL_TRUE, v.a));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseMtx23f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS == -1 && mFS == -1)
        return;

    const u32 NUM_COL = 3;

    f32 mat_trans[NUM_COL][4] = {
        { v.m[0][0], v.m[1][0] },
        { v.m[0][1], v.m[1][1] },
        { v.m[0][2], v.m[1][2] }
    };

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, NUM_COL*4, mat_trans);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, NUM_COL*4, mat_trans);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix3x2fv(location, 1, GL_TRUE, v.a));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseMtx24f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS == -1 && mFS == -1)
        return;

    const u32 NUM_COL = 4;

    f32 mat_trans[NUM_COL][4] = {
        { v.m[0][0], v.m[1][0] },
        { v.m[0][1], v.m[1][1] },
        { v.m[0][2], v.m[1][2] },
        { v.m[0][3], v.m[1][3] }
    };

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, NUM_COL*4, mat_trans);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, NUM_COL*4, mat_trans);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix4x2fv(location, 1, GL_TRUE, v.a));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseMtx32f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS == -1 && mFS == -1)
        return;

    const u32 NUM_COL = 2;

    f32 mat_trans[NUM_COL][4] = {
        { v.m[0][0], v.m[1][0], v.m[2][0] },
        { v.m[0][1], v.m[1][1], v.m[2][1] }
    };

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, NUM_COL*4, mat_trans);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, NUM_COL*4, mat_trans);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix2x3fv(location, 1, GL_TRUE, v.a));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseMtx33f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS == -1 && mFS == -1)
        return;

    const u32 NUM_COL = 3;

    f32 mat_trans[NUM_COL][4] = {
        { v.m[0][0], v.m[1][0], v.m[2][0] },
        { v.m[0][1], v.m[1][1], v.m[2][1] },
        { v.m[0][2], v.m[1][2], v.m[2][2] }
    };

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, NUM_COL*4, mat_trans);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, NUM_COL*4, mat_trans);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix3fv(location, 1, GL_TRUE, v.a));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseMtx34f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS == -1 && mFS == -1)
        return;

    const u32 NUM_COL = 4;

    f32 mat_trans[NUM_COL][4] = {
        { v.m[0][0], v.m[1][0], v.m[2][0] },
        { v.m[0][1], v.m[1][1], v.m[2][1] },
        { v.m[0][2], v.m[1][2], v.m[2][2] },
        { v.m[0][3], v.m[1][3], v.m[2][3] }
    };

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, NUM_COL*4, mat_trans);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, NUM_COL*4, mat_trans);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix4x3fv(location, 1, GL_TRUE, v.a));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseMtx42f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS == -1 && mFS == -1)
        return;

    const u32 NUM_COL = 2;

    f32 mat_trans[NUM_COL][4] = {
        { v.m[0][0], v.m[1][0], v.m[2][0], v.m[3][0] },
        { v.m[0][1], v.m[1][1], v.m[2][1], v.m[3][1] }
    };

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, NUM_COL*4, mat_trans);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, NUM_COL*4, mat_trans);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix2x4fv(location, 1, GL_TRUE, v.a));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseMtx43f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS == -1 && mFS == -1)
        return;

    const u32 NUM_COL = 3;

    f32 mat_trans[NUM_COL][4] = {
        { v.m[0][0], v.m[1][0], v.m[2][0], v.m[3][0] },
        { v.m[0][1], v.m[1][1], v.m[2][1], v.m[3][1] },
        { v.m[0][2], v.m[1][2], v.m[2][2], v.m[3][2] }
    };

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, NUM_COL*4, mat_trans);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, NUM_COL*4, mat_trans);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix3x4fv(location, 1, GL_TRUE, v.a));

#endif
}

inline
void UniformLocation::setUniform(const rio::BaseMtx44f& v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    if (mVS == -1 && mFS == -1)
        return;

    const u32 NUM_COL = 4;

    f32 mat_trans[NUM_COL][4] = {
        { v.m[0][0], v.m[1][0], v.m[2][0], v.m[3][0] },
        { v.m[0][1], v.m[1][1], v.m[2][1], v.m[3][1] },
        { v.m[0][2], v.m[1][2], v.m[2][2], v.m[3][2] },
        { v.m[0][3], v.m[1][3], v.m[2][3], v.m[3][3] }
    };

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, NUM_COL*4, mat_trans);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, NUM_COL*4, mat_trans);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix4fv(location, 1, GL_TRUE, v.a));

#endif
}

// GLSL type: float[]
//inline
//void UniformLocation::setUniformArray(u32 count, f32* v)

// GLSL type: int[]
//inline
//void UniformLocation::setUniformArray(u32 count, s32* v)

// GLSL type: uint[]
//inline
//void UniformLocation::setUniformArray(u32 count, u32* v)

// GLSL type: vec2[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseVec2f* v)

// GLSL type: ivec2[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseVec2i* v)

// GLSL type: uvec2[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseVec2u* v)

// GLSL type: vec3[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseVec3f* v)

// GLSL type: ivec3[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseVec3i* v)

// GLSL type: uvec3[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseVec3u* v)

inline
void UniformLocation::setUniformArray(u32 count, const rio::BaseVec4f* v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    RIO_ASSERT(count);
    RIO_ASSERT(v);

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, count*4, &v[0].x);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, count*4, &v[0].x);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_ASSERT(count);
    RIO_ASSERT(v);

    RIO_GL_CALL(glUniform4fv(location, count, &v[0].x));

#endif
}

inline
void UniformLocation::setUniformArray(u32 count, const rio::BaseVec4i* v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    RIO_ASSERT(count);
    RIO_ASSERT(v);

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, count*4, &v[0].x);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, count*4, &v[0].x);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_ASSERT(count);
    RIO_ASSERT(v);

    RIO_GL_CALL(glUniform4iv(location, count, &v[0].x));

#endif
}

inline
void UniformLocation::setUniformArray(u32 count, const rio::BaseVec4u* v)
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_CAFE

    RIO_ASSERT(count);
    RIO_ASSERT(v);

    if (mVS != -1)
        GX2SetVertexUniformReg(mVS, count*4, &v[0].x);

    if (mFS != -1)
        GX2SetPixelUniformReg(mFS, count*4, &v[0].x);

#elif RIO_IS_WIN

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_ASSERT(count);
    RIO_ASSERT(v);

    RIO_GL_CALL(glUniform4uiv(location, count, &v[0].x));

#endif
}

// GLSL type: mtx2[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseMtx22f* v)

// GLSL type: mtx32[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseMtx23f* v)

// GLSL type: mtx42[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseMtx24f* v)

// GLSL type: mtx23[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseMtx32f* v)

// GLSL type: mtx3[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseMtx33f* v)

// GLSL type: mtx43[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseMtx34f* v)

// GLSL type: mtx24[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseMtx42f* v)

// GLSL type: mtx34[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseMtx43f* v)

// GLSL type: mtx4[]
//inline
//void UniformLocation::setUniformArray(u32 count, const rio::BaseMtx44f* v)

}
