#include <common/aglRenderBuffer.h>
#include <common/aglRenderTarget.h>
#include <gfx/rio_Graphics.h>

namespace agl {

RenderBuffer::RenderBuffer()
    : mSize{1, 1}
    , mColorTarget(nullptr)
    , mDepthTarget(nullptr)
{
#if RIO_IS_WIN
    RIO_GL_CALL(glGenFramebuffers(1, &mHandle));
    RIO_ASSERT(mHandle != GL_NONE);
#endif // RIO_IS_WIN
}

RenderBuffer::RenderBuffer(const rio::Vector2i& size)
    : mSize{size}
    , mColorTarget(nullptr)
    , mDepthTarget(nullptr)
{
#if RIO_IS_WIN
    RIO_GL_CALL(glGenFramebuffers(1, &mHandle));
    RIO_ASSERT(mHandle != GL_NONE);
#endif // RIO_IS_WIN
}

RenderBuffer::~RenderBuffer()
{
    destroy();
}

void RenderBuffer::destroy()
{
#if RIO_IS_WIN
    if (mHandle != GL_NONE)
    {
        RIO_GL_CALL(glDeleteFramebuffers(1, &mHandle));
        mHandle = GL_NONE;
    }
#endif // RIO_IS_WIN

    mColorTarget = nullptr;
    mDepthTarget = nullptr;
}

void RenderBuffer::bind() const
{
    rio::Graphics::setViewport(0, 0, mSize.x, mSize.y);
    rio::Graphics::setScissor(0, 0, mSize.x, mSize.y);

#if RIO_IS_WIN
    RIO_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, mHandle));
#endif // RIO_IS_WIN

    if (mColorTarget)
        mColorTarget->bind();

    if (mDepthTarget)
        mDepthTarget->bind();
}

}
