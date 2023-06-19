#pragma once

#include <common/aglVertexBuffer.h>
#include <common/aglIndexStream.h>
#include <container/Buffer.h>
#include <container/SafeArray.h>
#include <math/rio_Vector.h>

namespace agl { namespace utl {

class VertexAttributeHolder;

class PrimitiveShape
{
public:
    static bool createSingleton();
    static void destroySingleton();
    static PrimitiveShape* instance() { return sInstance; }

private:
    static PrimitiveShape* sInstance;

    PrimitiveShape(const PrimitiveShape&);
    PrimitiveShape& operator=(const PrimitiveShape&);

public:
    enum DrawType
    {
        cDrawType_Triangle = 0,
        cDrawType_Line,
        cDrawType_Point,
        cDrawType_Num
    };
    static_assert(cDrawType_Num == 3);

    enum Quality
    {
        cQuality_0 = 0,
        cQuality_1,
        cQuality_2,
        cQuality_Num
    };
    static_assert(cQuality_Num == 3);

    struct Vertex
    {
        rio::Vector3f pos;
        rio::Vector3f nrm;
        rio::Vector2f tex;
    };
    static_assert(sizeof(Vertex) == 0x20, "agl::utl::PrimitiveShape::Vertex size mismatch");

public:
    PrimitiveShape();
    ~PrimitiveShape();

    void initialize();

    const IndexStream& getIdxStreamQuadTriangle(DrawType draw_type = cDrawType_Triangle) const
    {
        if (draw_type == cDrawType_Triangle)
            return mIdxStreamQuadTriangle;

        else // if (draw_type == cDrawType_Line)
            return mIdxStreamLineQuadTriangle;
    }

private:
    void setUpStreamQuad_();
    void setUpStreamQuadTriangle_();

    /*
    void setUpStreamCube_();
    void setUpStreamCircle_(u32);
    void setUpStreamSphere_(u32, u32);
    void setUpStreamHemisphere_(u32, u32);
    void setUpStreamCylinder_(u32, u32);
    void setUpStreamCone_(u32, u32);
    void setUpStreamTorus_(u32, u32, f32, f32, s32, s32);
    */

    static void setUpStreams_(VertexBuffer* p_vtx_buffer);

private:
    // Quad
    Buffer<Vertex>  mVtxQuad;
    Buffer<u32>     mIdxQuad;
    Buffer<u32>     mIdxLineQuad;
    VertexBuffer    mVtxBufferQuad;
    IndexStream     mIdxStreamQuad;
    IndexStream     mIdxStreamLineQuad;

    // QuadTriangle
    Buffer<Vertex>  mVtxQuadTriangle;
    Buffer<u32>     mIdxQuadTriangle;
    Buffer<u32>     mIdxLineQuadTriangle;
    VertexBuffer    mVtxBufferQuadTriangle;
    IndexStream     mIdxStreamQuadTriangle;
    IndexStream     mIdxStreamLineQuadTriangle;

    /*

    // Cube
    Buffer<Vertex>  mVtxCube;
    Buffer<u32>     mIdxCube;
    Buffer<u32>     mIdxLineCube;
    VertexBuffer    mVtxBufferCube;
    IndexStream     mIdxStreamCube;
    IndexStream     mIdxStreamLineCube;

    // Circle
    Buffer<Vertex>                                                          mVtxCircle;
    UnsafeArray<UnsafeArray<Buffer<u16>,    cDrawType_Num>, cQuality_Num>   mIdxCircle;
    VertexBuffer                                                            mVtxBufferCircle;
    UnsafeArray<UnsafeArray<IndexStream,    cDrawType_Num>, cQuality_Num>   mIdxStreamCircle;

    // Sphere
    Buffer<Vertex>                                                          mVtxSphere;
    UnsafeArray<UnsafeArray<Buffer<u16>,    cDrawType_Num>, cQuality_Num>   mIdxSphere;
    VertexBuffer                                                            mVtxBufferSphere;
    UnsafeArray<UnsafeArray<IndexStream,    cDrawType_Num>, cQuality_Num>   mIdxStreamSphere;

    // Hemisphere
    Buffer<Vertex>                                                          mVtxHemisphere;
    UnsafeArray<UnsafeArray<Buffer<u16>,    cDrawType_Num>, cQuality_Num>   mIdxHemisphere;
    VertexBuffer                                                            mVtxBufferHemisphere;
    UnsafeArray<UnsafeArray<IndexStream,    cDrawType_Num>, cQuality_Num>   mIdxStreamHemisphere;

    // Cylinder
    Buffer<Vertex>                                                          mVtxCylinder;
    UnsafeArray<Buffer<u32>,                                cQuality_Num>   mIdxCylinder;
    UnsafeArray<Buffer<u32>,                                cQuality_Num>   mIdxLineCylinder;
    VertexBuffer                                                            mVtxBufferCylinder;
    UnsafeArray<IndexStream,                                cQuality_Num>   mIdxStreamCylinder;
    UnsafeArray<IndexStream,                                cQuality_Num>   mIdxStreamLineCylinder;

    // Cone
    Buffer<Vertex>                                                          mVtxCone;
    UnsafeArray<Buffer<u32>,                                cQuality_Num>   mIdxCone;
    UnsafeArray<Buffer<u32>,                                cQuality_Num>   mIdxLineCone;
    VertexBuffer                                                            mVtxBufferCone;
    UnsafeArray<IndexStream,                                cQuality_Num>   mIdxStreamCone;
    UnsafeArray<IndexStream,                                cQuality_Num>   mIdxStreamLineCone;

    // Torus
    Buffer<Vertex>                                                          mVtxTorus;
    UnsafeArray<Buffer<u32>,                                cQuality_Num>   mIdxTorus;
    UnsafeArray<Buffer<u32>,                                cQuality_Num>   mIdxLineTorus;
    VertexBuffer                                                            mVtxBufferTorus;
    UnsafeArray<IndexStream,                                cQuality_Num>   mIdxStreamTorus;
    UnsafeArray<IndexStream,                                cQuality_Num>   mIdxStreamLineTorus;

    */

    friend class VertexAttributeHolder;
};
//static_assert(sizeof(PrimitiveShape) == 0x172C, "agl::utl::PrimitiveShape size mismatch");

} }
