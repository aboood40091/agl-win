#pragma once

#include <misc/rio_MemUtil.h>

#include <new>

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
    class iterator;
    class constIterator;

    class iterator
    {
    public:
        iterator()
            : mPtr(nullptr) // mIndex(0)
            , mBuffer(nullptr)
        {
        }

        explicit iterator(Buffer* buffer)
            : mPtr(buffer->mBuffer) // mIndex(0)
            , mBuffer(buffer->mBuffer)
        {
        }

        iterator(Buffer* buffer, s32 index)
            : mPtr(&buffer->mBuffer[index]) // mIndex(index)
            , mBuffer(buffer->mBuffer)
        {
        }

        iterator& operator++()
        {
            mPtr++; // mIndex++;
            return *this;
        }

        iterator& operator--()
        {
            mPtr--; // mIndex--;
            return *this;
        }

        friend bool operator==(const iterator& lhs, const iterator& rhs)
        {
            return lhs.mPtr == rhs.mPtr; // lhs.mIndex == rhs.mIndex;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs)
        {
            return lhs.mPtr != rhs.mPtr; // lhs.mIndex != rhs.mIndex;
        }

        T& operator*() const { return *mPtr; } // mBuffer[mIndex];
        T* operator->() const { return mPtr; } // &(mBuffer[mIndex]);

        s32 getIndex() const { return (uintptr_t(mPtr) - uintptr_t(mBuffer)) / sizeof(T); } // mIndex;

    private:
        T* mPtr; // int mIndex;
        T* mBuffer;

        friend class constIterator;
    };

    class constIterator
    {
    public:
        constIterator()
            : mPtr(nullptr) // mIndex(0)
            , mBuffer(nullptr)
        {
        }

        explicit constIterator(const Buffer* buffer)
            : mPtr(buffer->mBuffer) // mIndex(0)
            , mBuffer(buffer->mBuffer)
        {
        }

        constIterator(const Buffer* buffer, s32 index)
            : mPtr(&buffer->mBuffer[index]) // mIndex(index)
            , mBuffer(buffer->mBuffer)
        {
        }

        explicit constIterator(iterator it)
            : mPtr(it.mPtr) // mIndex(it.mIndex)
            , mBuffer(it.mBuffer)
        {
        }

        constIterator& operator++()
        {
            mPtr++; // mIndex++;
            return *this;
        }

        constIterator& operator--()
        {
            mPtr--; // mIndex--;
            return *this;
        }

        friend bool operator==(const constIterator& lhs, const constIterator& rhs)
        {
            return lhs.mPtr == rhs.mPtr; // lhs.mIndex == rhs.mIndex;
        }

        friend bool operator!=(const constIterator& lhs, const constIterator& rhs)
        {
            return lhs.mPtr != rhs.mPtr; // lhs.mIndex != rhs.mIndex;
        }

        const T& operator*() const { return *mPtr; } // mBuffer[mIndex];
        const T* operator->() const { return mPtr; } // &(mBuffer[mIndex]);

        s32 getIndex() const { return (uintptr_t(mPtr) - uintptr_t(mBuffer)) / sizeof(T); } // mIndex;

    private:
        const T* mPtr; // int mIndex;
        const T* mBuffer;
    };

public:
    iterator begin() { return iterator(this); }
    constIterator begin() const { return constIterator(this); }

    iterator end() { return iterator(this, mSize); }
    constIterator end() const { return constIterator(this, mSize); }

    iterator toIterator(s32 x)
    {
        if (static_cast<u32>(x) <= static_cast<u32>(mSize))
        {
            return iterator(this, x);
        }
        else
        {
            RIO_LOG("range over [0,%d] : %d\n", mSize, x);
            RIO_ASSERT(false);
            return end();
        }
    }

    constIterator toIterator(s32 x) const
    {
        if (static_cast<u32>(x) <= static_cast<u32>(mSize))
        {
            return constIterator(this, x);
        }
        else
        {
            RIO_LOG("range over [0,%d] : %d\n", mSize, x);
            RIO_ASSERT(false);
            return end();
        }
    }

    constIterator constBegin() const { return constIterator(this); }

    constIterator constEnd() const { return constIterator(this, mSize); }

    constIterator toConstIterator(s32 x) const
    {
        if (static_cast<u32>(x) <= static_cast<u32>(mSize))
        {
            return constIterator(this, x);
        }
        else
        {
            RIO_LOG("range over [0,%d] : %d\n", mSize, x);
            RIO_ASSERT(false);
            return constEnd();
        }
    }

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
        }
        else
        {
            RIO_LOG("size[%d] must be larger than zero\n", size);
            RIO_ASSERT(false);
        }
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
        else
        {
            RIO_LOG("size[%d] must be larger than zero\n", size);
            RIO_ASSERT(false);
            return false;
        }
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
