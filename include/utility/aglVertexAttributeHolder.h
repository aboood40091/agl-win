#pragma once

#include <common/aglVertexAttribute.h>
#include <container/SafeArray.h>

namespace agl { namespace utl {

class VertexAttributeHolder
{
public:
    static bool createSingleton();
    static void destroySingleton();
    static VertexAttributeHolder* instance() { return sInstance; }

private:
    static VertexAttributeHolder* sInstance;

    VertexAttributeHolder(const VertexAttributeHolder&);
    VertexAttributeHolder& operator=(const VertexAttributeHolder&);

public:
    enum VertexAttributeType
    {
      //cAttribute_Cube,
      //cAttribute_Cube2,
      //cAttribute_Cone,
      //cAttribute_Cylinder,
      //cAttribute_Sphere,
      //cAttribute_Sphere2,
        cAttribute_Quad,
        cAttribute_QuadTriangle,
      //cAttribute_Circle,
        cAttribute_Num
    };
  //static_assert(cAttribute_Num == 9);

public:
    VertexAttributeHolder();
    virtual ~VertexAttributeHolder();

    void initialize();

    const VertexAttribute& getVertexAttribute(VertexAttributeType type) const
    {
        return mVertexAttribute[type];
    }

private:
    SafeArray<VertexAttribute, cAttribute_Num> mVertexAttribute;
};
//static_assert(sizeof(VertexAttributeHolder) == 0x8A8, "agl::utl::VertexAttributeHolder size mismatch");

} }
