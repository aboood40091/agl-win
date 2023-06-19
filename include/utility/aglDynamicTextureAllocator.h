#pragma once

#include <common/aglTextureData.h>
#include <container/SafeArray.h>
#include <misc/rio_BitFlag.h>

namespace agl { namespace utl {

class DynamicTextureAllocator
{
public:
    static TextureData* alloc(
        const char* name,
        TextureFormat format,
        u32 width, u32 height, u32 mip_level_num
    );

    static void free(const TextureData* ptr);
};
//static_assert(sizeof(DynamicTextureAllocator) == 0x247C, "agl::utl::DynamicTextureAllocator size mismatch");

class TextureDataEx : public TextureData
{
public:
    TextureDataEx()
        : mName(nullptr)
    {
    }

private:
    const char* mName;

    friend class DynamicTextureAllocator;
};
//static_assert(sizeof(TextureDataEx) == 0xBC, "agl::utl::TextureDataEx size mismatch");

} }
