#pragma once

#include <common/aglShaderEnum.h>
#include <util/common/aglShaderOptimizeInfo.h>

namespace agl { namespace driver {

class GX2Resource
{
public:
    static bool createSingleton();
    static void destroySingleton();
    static GX2Resource* instance() { return sInstance; }

private:
    static GX2Resource* sInstance;

    GX2Resource();
    ~GX2Resource();

    GX2Resource(const GX2Resource&);
    GX2Resource& operator=(const GX2Resource&);

public:
    void initialize(u32 gs_in_ring_item_size = 0x10, u32 gs_out_ring_item_size = 0x80);

    void setShaderMode(ShaderMode mode) const;
    void setShaderMode(ShaderMode mode, const ShaderOptimizeInfo& info) const;

    void setGeometryShaderRingBuffer() const;

  //void restoreContextState();

private:
    ShaderOptimizeInfo* mpOptimizeInfo;
    ShaderOptimizeInfo mDefaultOptimizeInfo;
    size_t mGeometryShaderInputRingItemSize;
    size_t mGeometryShaderOutputRingItemSize;
    u8* mGeometryShaderInputRingBuffer;
    size_t mGeometryShaderInputRingBufferSize;
    u8* mGeometryShaderOutputRingBuffer;
    size_t mGeometryShaderOutputRingBufferSize;
    bool mStateShadowEnable;
    bool mUseStateDisplayList;
};
//static_assert(sizeof(GX2Resource) == 0x80, "agl::driver::GX2Resource size mismatch");

} }
