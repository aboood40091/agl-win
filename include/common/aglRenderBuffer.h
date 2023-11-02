#pragma once

#include <math/rio_Vector.h>

namespace agl {

class RenderTargetColor;
class RenderTargetDepth;

class RenderBuffer
{
public:
    RenderBuffer();
    RenderBuffer(const rio::Vector2i& size);
    ~RenderBuffer();

    void destroy();

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
    u32                 mHandle;
#endif // RIO_IS_WIN
};
//static_assert(sizeof(RenderBuffer) == 0x50, "agl::RenderBuffer size mistmatch");

}
