#pragma once

#include <misc/rio_Types.h>

class HashCRC32
{
public:
    static void initialize();
    static u32 calcHash(const void* dataptr, u32 datasize);

private:
    static u32 sTable[256];
    static bool sInitialized;
};
