#pragma once

#include <misc/rio_MemUtil.h>

#include <gsl/span>

template <typename T>
class Buffer
{
public:
    Buffer()
        : mSize(0)
        , mBuffer(nullptr)
    {
    }

    Buffer(s32 size, T* bufferptr)
    {
        if (size > 0 && bufferptr)
        {
            mSize = size;
            mBuffer = bufferptr;
        }
        else
        {
            RIO_LOG("illegal param: size[%d] bufferptr[0x%x]\n", size, uintptr_t(bufferptr));
            RIO_ASSERT(false);

            size = 0;
            bufferptr = nullptr;
        }
    }

public:
    class iterator
    {
    public:
        explicit iterator(T* buffer, s32 index = 0)
            : mPtr(buffer + index)
            , mStart(buffer)
        {
        }

        bool operator==(const iterator& rhs) const
        {
            return mPtr == rhs.mPtr;
        }

        bool operator!=(const iterator& rhs) const
        {
            return mPtr != rhs.mPtr;
        }

        iterator& operator++()
        {
            mPtr++;
            return *this;
        }

        T& operator*() const { return *mPtr; }
        T* operator->() const { return mPtr; }

        s32 getIndex() const { return ((uintptr_t)mPtr - (uintptr_t)mStart) / sizeof(T); }

    private:
        T* mPtr;
        T* mStart;
    };

    class constIterator
    {
    public:
        explicit constIterator(const T* buffer, s32 index = 0)
            : mPtr(buffer + index)
            , mStart(buffer)
        {
        }

        bool operator==(const constIterator& rhs) const
        {
            return mPtr == rhs.mPtr;
        }

        bool operator!=(const constIterator& rhs) const
        {
            return mPtr != rhs.mPtr;
        }

        constIterator& operator++()
        {
            mPtr++;
            return *this;
        }

        const T& operator*() const { return *mPtr; }
        const T* operator->() const { return mPtr; }

        s32 getIndex() const { return ((uintptr_t)mPtr - (uintptr_t)mStart) / sizeof(T); }

    private:
        const T* mPtr;
        const T* mStart;
    };

public:
    iterator begin() { return iterator(mBuffer); }
    constIterator begin() const { return constIterator(mBuffer); }

    iterator end() { return iterator(mBuffer, mSize); }
    constIterator end() const { return constIterator(mBuffer, mSize); }

    constIterator constBegin() const { return constIterator(mBuffer); }

    constIterator constEnd() const { return constIterator(mBuffer, mSize); }

public:
    void allocBuffer(s32 size, s32 alignment = 4)
    {
        RIO_ASSERT(mBuffer == nullptr);

        if (size > 0)
        {
            T* buffer = static_cast<T*>(rio::MemUtil::alloc(size * sizeof(T), alignment));

            for (s32 i = 0; i < size; i++)
                new (static_cast<void*>(&buffer[i])) T;

            setBuffer(size, buffer);
            return;
        }

        RIO_LOG("size[%d] must be larger than zero\n", size);
        RIO_ASSERT(false);
    }

    bool tryAllocBuffer(s32 size, s32 alignment = 4)
    {
        RIO_ASSERT(mBuffer == nullptr);

        if (size > 0)
        {
            T* buffer = static_cast<T*>(rio::MemUtil::alloc(size * sizeof(T), alignment));

            for (s32 i = 0; i < size; i++)
                new (static_cast<void*>(&buffer[i])) T;

            setBuffer(size, buffer);
            return true;
        }

        return false;
    }

    void freeBuffer()
    {
        if (isBufferReady())
        {
            for (s32 i = 0; i < mSize; i++)
                (&mBuffer[i])->~T();

            rio::MemUtil::free(mBuffer);

            mBuffer = nullptr;
            mSize = 0;
        }
    }

    void setBuffer(s32 size, T* bufferptr)
    {
        if (size > 0)
        {
            if (bufferptr)
            {
                mSize = size;
                mBuffer = bufferptr;
            }
            else
            {
                RIO_LOG("bufferptr is null\n");
                RIO_ASSERT(false);
            }
        }
        else
        {
            RIO_LOG("size[%d] must be larger than zero\n", size);
            RIO_ASSERT(false);
        }
    }

    bool isBufferReady() const { return mBuffer != nullptr; }

    T& operator[] (s32 x)
    {
        if (static_cast<u32>(x) < static_cast<u32>(mSize))
        {
            return mBuffer[x];
        }
        else
        {
            RIO_LOG("index exceeded [%d/%d]\n", x, mSize);
            RIO_ASSERT(false);
            return mBuffer[0];
        }
    }

    const T& operator[] (s32 x) const
    {
        if (static_cast<u32>(x) < static_cast<u32>(mSize))
        {
            return mBuffer[x];
        }
        else
        {
            RIO_LOG("index exceeded [%d/%d]\n", x, mSize);
            RIO_ASSERT(false);
            return mBuffer[0];
        }
    }

    T* get(s32 x)
    {
        if (static_cast<u32>(x) < static_cast<u32>(mSize))
        {
            return &mBuffer[x];
        }
        else
        {
            RIO_LOG("index exceeded [%d/%d]\n", x, mSize);
            RIO_ASSERT(false);
            return nullptr;
        }
    }

    const T* get(s32 x) const
    {
        if (static_cast<u32>(x) < static_cast<u32>(mSize))
        {
            return &mBuffer[x];
        }
        else
        {
            RIO_LOG("index exceeded [%d/%d]\n", x, mSize);
            RIO_ASSERT(false);
            return nullptr;
        }
    }

    T* unsafeGet(s32 x)
    {
        if (!isRangeValid(x))
        {
            RIO_LOG("index exceeded [%d/%d]\n", x, mSize);
            RIO_ASSERT(false);
        }
        return &mBuffer[x];
    }

    const T* unsafeGet(s32 x) const
    {
        if (!isRangeValid(x))
        {
            RIO_LOG("index exceeded [%d/%d]\n", x, mSize);
            RIO_ASSERT(false);
        }
        return &mBuffer[x];
    }

    T& front()
    {
        return mBuffer[0];
    }

    const T& front() const
    {
        return mBuffer[0];
    }

    T& back()
    {
        return mBuffer[mSize - 1];
    }

    const T& back() const
    {
        return mBuffer[mSize - 1];
    }

    s32 size() const { return mSize; }

    s32 getSize() const { return mSize; }
    T* getBufferPtr() { return mBuffer; }
    const T* getBufferPtr() const { return mBuffer; }
    u32 getByteSize() const { return mSize * sizeof(T); }

    Buffer<T>& operator=(const Buffer<T>&);

    bool isRangeValid(s32 x) const
    {
        return static_cast<u32>(x) < static_cast<u32>(mSize);
    }

    void fill(const T& value)
    {
        s32 size = mSize;
        for (s32 i = 0; i < size; i++)
            mBuffer[i] = value;
    }

protected:
    s32 mSize;
    T* mBuffer;
};
