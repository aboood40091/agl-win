#pragma once

#include <math/rio_Vector.h>
#if RIO_IS_WIN
#include <misc/gl/rio_GL.h>

#include <memory>
#endif // RIO_IS_WIN

namespace agl {

#if RIO_IS_WIN

class RenderBufferHandle
{
public:
    RenderBufferHandle()
    {
        RIO_GL_CALL(glGenFramebuffers(1, &mHandle));
        RIO_ASSERT(mHandle != GL_NONE);
    }

    ~RenderBufferHandle()
    {
        if (mHandle != GL_NONE)
        {
            RIO_GL_CALL(glDeleteFramebuffers(1, &mHandle));
            mHandle = GL_NONE;
        }
    }

    void bind() const
    {
        RIO_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, mHandle));
    }

private:
    u32 mHandle;
};

#endif // RIO_IS_WIN

class RenderTargetColor;
class RenderTargetDepth;

class RenderBuffer
{
public:
    RenderBuffer();
    RenderBuffer(const rio::Vector2i& size);

public:
    RenderTargetColor* getRenderTargetColor() const
    {
        return mColorTarget;
    }

    RenderTargetDepth* getRenderTargetDepth() const
    {
        return mDepthTarget;
    }

    void setRenderTargetColor(RenderTargetColor* target)
    {
        mColorTarget = target;
    }

    void setRenderTargetDepth(RenderTargetDepth* target)
    {
        mDepthTarget = target;
    }

    void setRenderTargetColorNull() { setRenderTargetColor(nullptr); }
    void setRenderTargetDepthNull() { setRenderTargetDepth(nullptr); }

    const rio::Vector2i& getSize() const
    {
        return mSize;
    }

    void setSize(u32 w, u32 h)
    {
        mSize.set(w, h);
    }

    void setSize(const rio::Vector2i& size)
    {
        mSize = size;
    }

    void bind() const;

private:
    rio::Vector2i       mSize;
    RenderTargetColor*  mColorTarget;
    RenderTargetDepth*  mDepthTarget;
#if RIO_IS_WIN
    std::unique_ptr<RenderBufferHandle> mHandle;
#endif // RIO_IS_WIN
};
//static_assert(sizeof(RenderBuffer) == 0x50, "agl::RenderBuffer size mistmatch");

}
