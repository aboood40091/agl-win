#include <common/aglResCommon.h>

namespace agl {

void ModifyEndianU32(bool is_le, void* p_data, size_t size)
{
    RIO_ASSERT(p_data != nullptr);
    RIO_ASSERT(size % 4 == 0);

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    if (!is_le)
#else
    if (is_le)
#endif
        return;

    u32* p_data_u32 = static_cast<u32*>(p_data);
    u32 count = size / sizeof(u32);

    for (u32 i = 0; i < count; i++)
    {
        *p_data_u32 = __builtin_bswap32(*p_data_u32);
        p_data_u32++;
    }
}

}
