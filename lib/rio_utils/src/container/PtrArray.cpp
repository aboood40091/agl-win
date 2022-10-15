#include <container/PtrArray.h>

#include <misc/rio_MemUtil.h>

#include <algorithm>

void PtrArrayImpl::setBuffer(s32 ptrNumMax, void* buf)
{
    if (ptrNumMax > 0)
    {
        if (buf != NULL)
        {
            mPtrNumMax = ptrNumMax;
            mPtrs = static_cast<void**>(buf);
            mPtrNum = 0;
        }
        else
        {
            RIO_LOG("buf is null\n");
            RIO_ASSERT(false);
        }
    }
    else
    {
        RIO_LOG("ptrNumMax[%d] must be larger than zero\n", ptrNumMax);
        RIO_ASSERT(false);
    }
}

void PtrArrayImpl::allocBuffer(s32 ptrNumMax, s32 alignment)
{
    RIO_ASSERT(mPtrs == nullptr);
    if (ptrNumMax > 0)
        setBuffer(ptrNumMax, rio::MemUtil::alloc(sizeof(void*) * ptrNumMax, alignment));
    else
    {
        RIO_LOG("ptrNumMax[%d] must be larger than zero\n", ptrNumMax);
        RIO_ASSERT(false);
    }
}

void PtrArrayImpl::freeBuffer()
{
    if (mPtrs != NULL)
    {
        rio::MemUtil::free(mPtrs);
        mPtrs = NULL;
        mPtrNum = 0;
        mPtrNumMax = 0;
    }
}

void PtrArrayImpl::sort(CompareCallbackImpl cmp)
{
    for (s32 x = 0; x < mPtrNum - 1; x++)
    {
        s32 sorted = mPtrNum - x;
        for (s32 y = 0; y < sorted - 1; ++y)
            if ((*cmp)(mPtrs[y], mPtrs[y + 1]) > 0)
                std::swap(mPtrs[y], mPtrs[y + 1]);
    }
}
