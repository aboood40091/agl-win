#include <detail/aglFileIOMgr.h>
#include <detail/aglPrivateResource.h>
#include <util/common/aglShader.h>
#include <util/common/aglShaderCompileInfo.h>

#if RIO_IS_WIN
#include <filedevice/rio_FileDeviceMgr.h>
#endif // RIO_IS_WIN

namespace agl {

Shader::Shader()
    : mBinary(NULL)
    , mCompileInfo(NULL)
{
}

u32 Shader::setUp(bool compile_source, bool) const
{
    u32 ret = 2;

    if (mCompileInfo && compile_source)
    {
        std::string* temp_text = &detail::PrivateResource::sShaderText;

#if RIO_IS_CAFE || RIO_IS_WIN
        mCompileInfo->calcCompileSource(getShaderType(), temp_text, ShaderCompileInfo::cTarget_GX2, true);
#else
        mCompileInfo->calcCompileSource(getShaderType(), temp_text, ShaderCompileInfo::cTarget_GL, true);
#endif

#if RIO_IS_CAFE
        /*
        static const char* sSavePath[cShaderType_Num] = {
            "%AGL_ROOT%/tools/temporary/temp_vs.sh",
            "%AGL_ROOT%/tools/temporary/temp_fs.sh",
            "%AGL_ROOT%/tools/temporary/temp_gs.sh"
        };

        detail::FileIOMgr::DialogArg arg;
        arg.mPath = sSavePath[getShaderType()];

        detail::FileIOMgr::instance()->save(temp_text->c_str(), temp_text->length(), arg);
        */
#elif RIO_IS_WIN
        static const char* sSavePath[cShaderType_Num] = {
            "shaders/agl_shader_temp.vert",
            "shaders/agl_shader_temp.frag",
            "shaders/agl_shader_temp.geom",
        };

        RIO_ASSERT(getShaderType() != agl::cShaderType_Geometry);

        rio::FileHandle handle;

        [[maybe_unused]] rio::FileDevice* device = rio::FileDeviceMgr::instance()->open(&handle, sSavePath[getShaderType()], rio::FileDevice::FILE_OPEN_FLAG_WRITE);
        RIO_ASSERT(device != nullptr);

        handle.write(reinterpret_cast<const u8*>(temp_text->c_str()), temp_text->length());
        handle.close();
#endif

        ret = 0;
    }

    if (mBinary)
        ret = 0;

    return ret;
}

void Shader::setBinary(const void* binary)
{
    mBinary = binary;
}

}
