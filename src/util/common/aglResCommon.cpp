#include <util/common/aglResCommon.h>

namespace agl {

void ModifyEndianU32(bool is_le, void* ptr, size_t size)
{
    u32* ptr_u32 = static_cast<u32*>(ptr);
    u32 count = size / sizeof(u32);

    for (u32 i = 0; i < count; i++)
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        if (is_le)
#else
        if (!is_le)
#endif
            *ptr_u32 = __builtin_bswap32(*ptr_u32);

        ptr_u32++;
    }
}

}
