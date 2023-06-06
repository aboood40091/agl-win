#include <common/aglDisplayList.h>
#include <detail/aglPrivateResource.h>

namespace agl {

DisplayList::DisplayList()
    : mSelfAlloc(false)
{
    clear();
}

DisplayList::~DisplayList()
{
    clear();
}

void DisplayList::setBuffer_(u8* buffer, size_t size, bool self_alloc)
{
    if (mSelfAlloc && mpBuffer)
        rio::MemUtil::free(mpBuffer);

    if (buffer)
        mSize = size;
    else
        mSize = 0;

    mpBuffer = buffer;
    mSelfAlloc = self_alloc;
}

void DisplayList::clear()
{
    if (mSelfAlloc && mpBuffer)
        rio::MemUtil::free(mpBuffer);

    mpBuffer = nullptr;
    mSize = 0;
    mValidSize = 0;
    mSelfAlloc = false;
}

bool DisplayList::beginDisplayList()
{
    mValidSize = 0;

#if RIO_IS_CAFE
    GX2BeginDisplayListEx(mpBuffer, mSize, GX2_DISABLE);
    return true;
#else
    return false;
#endif
}

size_t DisplayList::endDisplayList()
{
#if RIO_IS_CAFE
    void* dl;
    size_t size;
    GX2GetCurrentDisplayList(&dl, &size);
    size_t dl_size = GX2EndDisplayList(dl);
    mValidSize = dl_size + ((uintptr_t)dl - (uintptr_t)mpBuffer);
    return mValidSize;
#else
    return 0;
#endif
}

bool DisplayList::beginDisplayListBuffer(u8* buffer, size_t size)
{
    RIO_ASSERT(0 < size);
#if RIO_IS_CAFE
    DCInvalidateRange(buffer, size);
#endif // RIO_IS_CAFE
    setBuffer(buffer, size);
    return beginDisplayList();
}

size_t DisplayList::endDisplayListBuffer()
{
    u8* buffer = nullptr;
    size_t size = endDisplayList();
    bool self_alloc = false;
    if (size != 0)
    {
        buffer = static_cast<u8*>(rio::MemUtil::alloc(size, cDisplayListAlignment));
        rio::MemUtil::copy(buffer, mpBuffer, size);
#if RIO_IS_CAFE
        DCFlushRange(buffer, size);
#endif // cafe
        self_alloc = true;
    }
    setBuffer_(buffer, size, self_alloc);
    return size;
}

bool DisplayList::beginDisplayListTemporary(size_t size)
{
    u8* buffer = static_cast<u8*>(rio::MemUtil::alloc(size, cDisplayListAlignment));
    return beginDisplayListBuffer(buffer, size);
}

size_t DisplayList::endDisplayListTemporary()
{
    u8* buffer = mpBuffer;
    size_t size = endDisplayListBuffer();
    rio::MemUtil::free(buffer);
    return size;
}

size_t DisplayList::suspend(void** p_dl)
{
    RIO_ASSERT(p_dl != nullptr);

    *p_dl = nullptr;
    size_t size = 0;

#if RIO_IS_CAFE
    if (GX2GetCurrentDisplayList(p_dl, &size) == GX2_TRUE)
    {
        size_t dl_size = GX2EndDisplayList(*p_dl);

        size -= dl_size;
        *p_dl = (void*)((uintptr_t)*p_dl + dl_size);
    }
#endif // RIO_IS_CAFE

    return size;
}
void DisplayList::resume(void* dl, size_t size)
{
#if RIO_IS_CAFE
    if (dl != nullptr)
        GX2BeginDisplayListEx(dl, size, GX2_DISABLE);
#endif // RIO_IS_CAFE
}

}
