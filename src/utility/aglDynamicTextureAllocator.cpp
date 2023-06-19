#include <container/Buffer.h>
#include <math/rio_Math.h>
#include <utility/aglDynamicTextureAllocator.h>

namespace agl { namespace utl {

TextureData* DynamicTextureAllocator::alloc(
    const char* name,
    TextureFormat format,
    u32 width, u32 height, u32 mip_level_num
)
{
    TextureDataEx* p_tex = new TextureDataEx();
    RIO_ASSERT(p_tex != nullptr);
    p_tex->initialize(format, width, height, mip_level_num);

#if RIO_IS_CAFE

    p_tex->setImagePtr(rio::MemUtil::alloc(p_tex->getImageByteSize(), p_tex->getAlignment()));

    if (p_tex->getMipLevelNum() > 1)
        p_tex->setMipPtr(rio::MemUtil::alloc(p_tex->getMipByteSize(), p_tex->getAlignment()));
    else
        p_tex->setMipPtr(nullptr);

#elif RIO_IS_WIN

    const auto& handle = std::make_shared<TextureHandle>();
    handle->bind();

    RIO_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
    RIO_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, p_tex->getMipLevelNum() - 1));

    RIO_GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    const rio::NativeTextureFormat& native_format = p_tex->getNativeTextureFormat();

    switch (p_tex->getTextureFormat())
    {
    case cTextureFormat_BC1_uNorm:
    case cTextureFormat_BC2_uNorm:
    case cTextureFormat_BC3_uNorm:
    case cTextureFormat_BC4_uNorm:
    case cTextureFormat_BC4_sNorm:
    case cTextureFormat_BC5_uNorm:
    case cTextureFormat_BC5_sNorm:
        {
            RIO_GL_CALL(glCompressedTexImage2D(
                GL_TEXTURE_2D,
                0,
                native_format.internalformat,
                p_tex->getWidth(),
                p_tex->getHeight(),
                0,
                p_tex->getImageByteSize(),
                nullptr
            ));

            if (p_tex->getMipLevelNum() > 1)
            {
                for (u32 i = 0; i < p_tex->getMipLevelNum() - 2; i++)
                    RIO_GL_CALL(glCompressedTexImage2D(
                        GL_TEXTURE_2D,
                        i + 1,
                        native_format.internalformat,
                        p_tex->getWidth(i + 1),
                        p_tex->getHeight(i + 1),
                        0,
                        p_tex->getMipLevelByteSize(i + 1),
                        nullptr
                    ));

                RIO_GL_CALL(glCompressedTexImage2D(
                    GL_TEXTURE_2D,
                    (p_tex->getMipLevelNum() - 2) + 1,
                    native_format.internalformat,
                    p_tex->getWidth((p_tex->getMipLevelNum() - 2) + 1),
                    p_tex->getHeight((p_tex->getMipLevelNum() - 2) + 1),
                    0,
                    p_tex->getMipLevelByteSize((p_tex->getMipLevelNum() - 2) + 1),
                    nullptr
                ));
            }
        }
        break;
    case cTextureFormat_BC1_SRGB:
    case cTextureFormat_BC2_SRGB:
    case cTextureFormat_BC3_SRGB:
        {
            RIO_GL_CALL(glCompressedTexImage2DARB(
                GL_TEXTURE_2D,
                0,
                native_format.internalformat,
                p_tex->getWidth(),
                p_tex->getHeight(),
                0,
                p_tex->getImageByteSize(),
                nullptr
            ));

            if (p_tex->getMipLevelNum() > 1)
            {
                for (u32 i = 0; i < p_tex->getMipLevelNum() - 2; i++)
                    RIO_GL_CALL(glCompressedTexImage2DARB(
                        GL_TEXTURE_2D,
                        i + 1,
                        native_format.internalformat,
                        p_tex->getWidth(i + 1),
                        p_tex->getHeight(i + 1),
                        0,
                        p_tex->getMipLevelByteSize(i + 1),
                        nullptr
                    ));

                RIO_GL_CALL(glCompressedTexImage2DARB(
                    GL_TEXTURE_2D,
                    (p_tex->getMipLevelNum() - 2) + 1,
                    native_format.internalformat,
                    p_tex->getWidth((p_tex->getMipLevelNum() - 2) + 1),
                    p_tex->getHeight((p_tex->getMipLevelNum() - 2) + 1),
                    0,
                    p_tex->getMipLevelByteSize((p_tex->getMipLevelNum() - 2) + 1),
                    nullptr
                ));
            }
        }
        break;
    default:
        {
            RIO_GL_CALL(glTexStorage2D(
                GL_TEXTURE_2D,
                p_tex->getMipLevelNum(),
                native_format.internalformat,
                p_tex->getWidth(),
                p_tex->getHeight()
            ));
        }
        break;
    }

    p_tex->setHandle(handle);

#endif

    p_tex->mName = name;

    p_tex->invalidateGPUCache();

    return p_tex;
}

void DynamicTextureAllocator::free(const TextureData* ptr)
{
    RIO_ASSERT(ptr != nullptr);
    TextureDataEx* p_tex = static_cast<TextureDataEx*>(const_cast<TextureData*>(ptr));

#if RIO_IS_CAFE

    void* image_ptr = p_tex->getImagePtr();
    void* mip_ptr = p_tex->getMipPtr();

    RIO_ASSERT(image_ptr != nullptr);

    rio::MemUtil::free(image_ptr);
    if (mip_ptr)
        rio::MemUtil::free(mip_ptr);

    p_tex->setImagePtr(nullptr);
    p_tex->setMipPtr(nullptr);

#elif RIO_IS_WIN

    p_tex->setHandle(nullptr);

#endif

    delete p_tex;
}

} }
