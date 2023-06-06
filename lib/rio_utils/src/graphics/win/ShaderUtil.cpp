#include <graphics/win/ShaderUtil.h>
#include <misc/MD5.h>

#include <filedevice/rio_FileDeviceMgr.h>

#include <cafe/gfd.h>

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <windows.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace {

bool FileExists(const char* path)
{
    DWORD dwAttrib = GetFileAttributes(path);
    return dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool FolderExists(const char* path)
{
    DWORD dwAttrib = GetFileAttributes(path);
    return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void RunCommand(const char* cmd)
{
    STARTUPINFOA si = { sizeof(STARTUPINFOA), 0 };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = { 0 };

    if (CreateProcessA(NULL, const_cast<char*>(cmd), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

bool ReadFile(const char* filename, u8** out_data, u32* out_size)
{
    if (!out_data && !out_size)
        return false;

    std::ifstream inf(filename, std::ifstream::in | std::ifstream::binary);

    inf.seekg(0, std::ios::end);
    const u32 inf_size = inf.tellg();
    inf.seekg(0, std::ios::beg);

    if (inf_size == 0xFFFFFFFF)
    {
        inf.close();
        return false;
    }

    if (out_data)
    {
        u8* const inb = new u8[inf_size];
        if (!inb)
            return false;

        inf.read((char*)inb, inf_size);

        *out_data = inb;
    }

    if (out_size)
        *out_size = inf_size;

    inf.close();
    return true;
}

bool WriteFile(const char* filename, const u8* data, u32 size)
{
    if (!data || !size)
        return false;

    std::ofstream outf(filename, std::ofstream::out | std::ofstream::binary);

    outf.write((char*)data, size);
    outf.close();

    return true;
}

void FreeFile(const void* data)
{
    delete[] (u8*)data;
}

bool ReadFile(const std::string& filename, std::string* out_str)
{
    if (!out_str)
        return false;

    std::ifstream inf(filename.c_str(), std::ifstream::in | std::ifstream::binary);

    inf.seekg(0, std::ios::end);
    const u32 inf_size = inf.tellg();
    inf.seekg(0, std::ios::beg);

    if (inf_size == 0xFFFFFFFF)
    {
        inf.close();
        return false;
    }

    u8* const inb = new u8[inf_size];
    if (!inb)
        return false;

    inf.read((char*)inb, inf_size);
    inf.close();

    *out_str = std::string((const char*)inb, inf_size);

    delete[] inb;

    return true;
}

bool WriteFile(const std::string& filename, const std::string& str)
{
    return WriteFile(filename.c_str(), (const u8*)str.c_str(), str.size() + 1);
}

size_t SaveGX2VertexShader(
    GX2VertexShader* shader,
    void** pShaderBuf
)
{
    size_t shaderBufSize = sizeof(GX2VertexShader);

    const size_t uniformBlocksOffs = shaderBufSize;
    shaderBufSize += shader->numUniformBlocks * sizeof(GX2UniformBlock);

    const size_t uniformVarsOffs = shaderBufSize;
    shaderBufSize += shader->numUniforms * sizeof(GX2UniformVar);

    const size_t initialValuesOffs = shaderBufSize;
    shaderBufSize += shader->numInitialValues * sizeof(GX2UniformInitialValue);

    const size_t loopVarsOffs = shaderBufSize;
    shaderBufSize += shader->_numLoops * sizeof(u32) * 2;

    const size_t samplerVarsOffs = shaderBufSize;
    shaderBufSize += shader->numSamplers * sizeof(GX2SamplerVar);

    const size_t attribVarsOffs = shaderBufSize;
    shaderBufSize += shader->numAttribs * sizeof(GX2AttribVar);

    const size_t strBaseOffs = shaderBufSize;

    for (u32 i = 0; i < shader->numUniformBlocks; i++)
    {
        const char* const name = shader->uniformBlocks[i].name;
        if (name != NULL)
            shaderBufSize += std::strlen(name) + 1;
    }

    for (u32 i = 0; i < shader->numUniforms; i++)
    {
        const char* const name = shader->uniformVars[i].name;
        if (name != NULL)
            shaderBufSize += std::strlen(name) + 1;
    }

    for (u32 i = 0; i < shader->numSamplers; i++)
    {
        const char* const name = shader->samplerVars[i].name;
        if (name != NULL)
            shaderBufSize += std::strlen(name) + 1;
    }

    for (u32 i = 0; i < shader->numAttribs; i++)
    {
        const char* const name = shader->attribVars[i].name;
        if (name != NULL)
            shaderBufSize += std::strlen(name) + 1;
    }

    const size_t shaderDataOffs = shaderBufSize;
    shaderBufSize += shader->shaderSize;

    u8* shaderBuf = (u8*)rio::MemUtil::alloc(shaderBufSize, 4);

    std::memcpy(shaderBuf, shader, sizeof(GX2VertexShader));
    std::memcpy(shaderBuf + shaderDataOffs, shader->shaderPtr, shader->shaderSize);

    *(u32*)(shaderBuf + offsetof(GX2VertexShader, shaderPtr)) = shaderDataOffs;

    if (shader->numUniformBlocks != 0)
    {
        std::memcpy(shaderBuf + uniformBlocksOffs,
                    shader->uniformBlocks,
                    shader->numUniformBlocks * sizeof(GX2UniformBlock));

        *(u32*)(shaderBuf + offsetof(GX2VertexShader, uniformBlocks)) = uniformBlocksOffs;
    }

    if (shader->numUniforms != 0)
    {
        std::memcpy(shaderBuf + uniformVarsOffs,
                    shader->uniformVars,
                    shader->numUniforms * sizeof(GX2UniformVar));

        *(u32*)(shaderBuf + offsetof(GX2VertexShader, uniformVars)) = uniformVarsOffs;
    }

    if (shader->numInitialValues != 0)
    {
        std::memcpy(shaderBuf + initialValuesOffs,
                    shader->initialValues,
                    shader->numInitialValues * sizeof(GX2UniformInitialValue));

        *(u32*)(shaderBuf + offsetof(GX2VertexShader, initialValues)) = initialValuesOffs;
    }

    if (shader->_numLoops != 0)
    {
        std::memcpy(shaderBuf + loopVarsOffs,
                    shader->_loopVars,
                    shader->_numLoops * sizeof(u32) * 2);

        *(u32*)(shaderBuf + offsetof(GX2VertexShader, _loopVars)) = loopVarsOffs;
    }

    if (shader->numSamplers != 0)
    {
        std::memcpy(shaderBuf + samplerVarsOffs,
                    shader->samplerVars,
                    shader->numSamplers * sizeof(GX2SamplerVar));

        *(u32*)(shaderBuf + offsetof(GX2VertexShader, samplerVars)) = samplerVarsOffs;
    }

    if (shader->numAttribs != 0)
    {
        std::memcpy(shaderBuf + attribVarsOffs,
                    shader->attribVars,
                    shader->numAttribs * sizeof(GX2AttribVar));

        *(u32*)(shaderBuf + offsetof(GX2VertexShader, attribVars)) = attribVarsOffs;
    }

    size_t strOffs = strBaseOffs;

    for (u32 i = 0; i < shader->numUniformBlocks; i++)
    {
        const char* const name = shader->uniformBlocks[i].name;
        if (name != NULL)
        {
            const size_t nameLen = std::strlen(name) + 1;
            std::memcpy(shaderBuf + strOffs, name, nameLen);

            *(u32*)(shaderBuf +
                    uniformBlocksOffs
                    + i * sizeof(GX2UniformBlock)
                    + offsetof(GX2UniformBlock, name)) = strOffs;

            strOffs += nameLen;
        }
    }

    for (u32 i = 0; i < shader->numUniforms; i++)
    {
        const char* const name = shader->uniformVars[i].name;
        if (name != NULL)
        {
            const size_t nameLen = std::strlen(name) + 1;
            std::memcpy(shaderBuf + strOffs, name, nameLen);

            *(u32*)(shaderBuf +
                    uniformVarsOffs
                    + i * sizeof(GX2UniformVar)
                    + offsetof(GX2UniformVar, name)) = strOffs;

            strOffs += nameLen;
        }
    }

    for (u32 i = 0; i < shader->numSamplers; i++)
    {
        const char* const name = shader->samplerVars[i].name;
        if (name != NULL)
        {
            const size_t nameLen = std::strlen(name) + 1;
            std::memcpy(shaderBuf + strOffs, name, nameLen);

            *(u32*)(shaderBuf +
                    samplerVarsOffs
                    + i * sizeof(GX2SamplerVar)
                    + offsetof(GX2SamplerVar, name)) = strOffs;

            strOffs += nameLen;
        }
    }

    for (u32 i = 0; i < shader->numAttribs; i++)
    {
        const char* const name = shader->attribVars[i].name;
        if (name != NULL)
        {
            const size_t nameLen = std::strlen(name) + 1;
            std::memcpy(shaderBuf + strOffs, name, nameLen);

            *(u32*)(shaderBuf +
                    attribVarsOffs
                    + i * sizeof(GX2AttribVar)
                    + offsetof(GX2AttribVar, name)) = strOffs;

            strOffs += nameLen;
        }
    }

    *pShaderBuf = shaderBuf;
    return shaderBufSize;
}

size_t SaveGX2PixelShader(
    GX2PixelShader* shader,
    void** pShaderBuf
)
{
    size_t shaderBufSize = sizeof(GX2PixelShader);

    const size_t uniformBlocksOffs = shaderBufSize;
    shaderBufSize += shader->numUniformBlocks * sizeof(GX2UniformBlock);

    const size_t uniformVarsOffs = shaderBufSize;
    shaderBufSize += shader->numUniforms * sizeof(GX2UniformVar);

    const size_t initialValuesOffs = shaderBufSize;
    shaderBufSize += shader->numInitialValues * sizeof(GX2UniformInitialValue);

    const size_t loopVarsOffs = shaderBufSize;
    shaderBufSize += shader->_numLoops * sizeof(u32) * 2;

    const size_t samplerVarsOffs = shaderBufSize;
    shaderBufSize += shader->numSamplers * sizeof(GX2SamplerVar);

    const size_t strBaseOffs = shaderBufSize;

    for (u32 i = 0; i < shader->numUniformBlocks; i++)
    {
        const char* const name = shader->uniformBlocks[i].name;
        if (name != NULL)
            shaderBufSize += std::strlen(name) + 1;
    }

    for (u32 i = 0; i < shader->numUniforms; i++)
    {
        const char* const name = shader->uniformVars[i].name;
        if (name != NULL)
            shaderBufSize += std::strlen(name) + 1;
    }

    for (u32 i = 0; i < shader->numSamplers; i++)
    {
        const char* const name = shader->samplerVars[i].name;
        if (name != NULL)
            shaderBufSize += std::strlen(name) + 1;
    }

    const size_t shaderDataOffs = shaderBufSize;
    shaderBufSize += shader->shaderSize;

    u8* shaderBuf = (u8*)rio::MemUtil::alloc(shaderBufSize, 4);

    std::memcpy(shaderBuf, shader, sizeof(GX2PixelShader));
    std::memcpy(shaderBuf + shaderDataOffs, shader->shaderPtr, shader->shaderSize);

    *(u32*)(shaderBuf + offsetof(GX2PixelShader, shaderPtr)) = shaderDataOffs;

    if (shader->numUniformBlocks != 0)
    {
        std::memcpy(shaderBuf + uniformBlocksOffs,
                    shader->uniformBlocks,
                    shader->numUniformBlocks * sizeof(GX2UniformBlock));

        *(u32*)(shaderBuf + offsetof(GX2PixelShader, uniformBlocks)) = uniformBlocksOffs;
    }

    if (shader->numUniforms != 0)
    {
        std::memcpy(shaderBuf + uniformVarsOffs,
                    shader->uniformVars,
                    shader->numUniforms * sizeof(GX2UniformVar));

        *(u32*)(shaderBuf + offsetof(GX2PixelShader, uniformVars)) = uniformVarsOffs;
    }

    if (shader->numInitialValues != 0)
    {
        std::memcpy(shaderBuf + initialValuesOffs,
                    shader->initialValues,
                    shader->numInitialValues * sizeof(GX2UniformInitialValue));

        *(u32*)(shaderBuf + offsetof(GX2PixelShader, initialValues)) = initialValuesOffs;
    }

    if (shader->_numLoops != 0)
    {
        std::memcpy(shaderBuf + loopVarsOffs,
                    shader->_loopVars,
                    shader->_numLoops * sizeof(u32) * 2);

        *(u32*)(shaderBuf + offsetof(GX2PixelShader, _loopVars)) = loopVarsOffs;
    }

    if (shader->numSamplers != 0)
    {
        std::memcpy(shaderBuf + samplerVarsOffs,
                    shader->samplerVars,
                    shader->numSamplers * sizeof(GX2SamplerVar));

        *(u32*)(shaderBuf + offsetof(GX2PixelShader, samplerVars)) = samplerVarsOffs;
    }

    size_t strOffs = strBaseOffs;

    for (u32 i = 0; i < shader->numUniformBlocks; i++)
    {
        const char* const name = shader->uniformBlocks[i].name;
        if (name != NULL)
        {
            const size_t nameLen = std::strlen(name) + 1;
            std::memcpy(shaderBuf + strOffs, name, nameLen);

            *(u32*)(shaderBuf +
                    uniformBlocksOffs
                    + i * sizeof(GX2UniformBlock)
                    + offsetof(GX2UniformBlock, name)) = strOffs;

            strOffs += nameLen;
        }
    }

    for (u32 i = 0; i < shader->numUniforms; i++)
    {
        const char* const name = shader->uniformVars[i].name;
        if (name != NULL)
        {
            const size_t nameLen = std::strlen(name) + 1;
            std::memcpy(shaderBuf + strOffs, name, nameLen);

            *(u32*)(shaderBuf +
                    uniformVarsOffs
                    + i * sizeof(GX2UniformVar)
                    + offsetof(GX2UniformVar, name)) = strOffs;

            strOffs += nameLen;
        }
    }

    for (u32 i = 0; i < shader->numSamplers; i++)
    {
        const char* const name = shader->samplerVars[i].name;
        if (name != NULL)
        {
            const size_t nameLen = std::strlen(name) + 1;
            std::memcpy(shaderBuf + strOffs, name, nameLen);

            *(u32*)(shaderBuf +
                    samplerVarsOffs
                    + i * sizeof(GX2SamplerVar)
                    + offsetof(GX2SamplerVar, name)) = strOffs;

            strOffs += nameLen;
        }
    }

    *pShaderBuf = shaderBuf;
    return shaderBufSize;
}

bool GX2UniformBlockComp(const GX2UniformBlock& a, const GX2UniformBlock& b)
{
    return a.location > b.location;
}

void ReplaceString(std::string& str, const std::string& a, const std::string& b)
{
    size_t pos = 0;
    while ((pos = str.find(a, pos)) != std::string::npos)
    {
         str.replace(pos, a.length(), b);
         pos += b.length();
    }
}

struct ShaderCache
{
    ShaderCache(const std::string& v, const std::string& f)
        : vertexShader(v)
        , fragmentShader(f)
    {
    }

    ~ShaderCache()
    {
    }

    std::string vertexShader;
    std::string fragmentShader;
};
typedef std::unordered_map<std::string, const ShaderCache> ShaderCacheMap;
ShaderCacheMap sShaderCache;

bool DecompileProgram(
    GX2VertexShader* vertexShader,
    GX2PixelShader* pixelShader,
    std::string* vertexShaderSrc,
    std::string* fragmentShaderSrc
)
{
    void* vertexShaderBuf;
    size_t vertexShaderBufSize = SaveGX2VertexShader(vertexShader, &vertexShaderBuf);

    void* pixelShaderBuf;
    size_t pixelShaderBufSize = SaveGX2PixelShader(pixelShader, &pixelShaderBuf);

    u8* shaderBuf = new u8[vertexShaderBufSize + pixelShaderBufSize];
    std::memcpy(shaderBuf, vertexShaderBuf, vertexShaderBufSize);
    std::memcpy(shaderBuf + vertexShaderBufSize, pixelShaderBuf, pixelShaderBufSize);

    std::string key = md5(shaderBuf, vertexShaderBufSize + pixelShaderBufSize);

    std::string glVertexShader;
    std::string glFragmentShader;

    ShaderCacheMap::const_iterator it = sShaderCache.find(key);
    if (it != sShaderCache.end())
    {
      //RIO_LOG("  Loading cache from map\n");
        const ShaderCache& shaderCache = it->second;
        glVertexShader = shaderCache.vertexShader;
        glFragmentShader = shaderCache.fragmentShader;
    }
    else
    {
        const std::string& basePath = ShaderUtil::sTempPath + '/' + key;

        const std::string& vertexShaderPath = basePath + "VS";
        const std::string& fragmentShaderPath = basePath + "FS";

        const std::string& vertexShaderSrcPath = vertexShaderPath + ".vert";
        const std::string& fragmentShaderSrcPath = fragmentShaderPath + ".frag";

        if (FileExists(vertexShaderSrcPath.c_str()) && FileExists(fragmentShaderSrcPath.c_str()))
        {
          //RIO_LOG("  Loading cache from disk\n");
            {
                [[maybe_unused]] bool read = ReadFile(vertexShaderSrcPath, &glVertexShader);
                RIO_ASSERT(read);
            }
            {
                [[maybe_unused]] bool read = ReadFile(fragmentShaderSrcPath, &glFragmentShader);
                RIO_ASSERT(read);
            }
        }
        else
        {
            const std::string& vertexShaderSpirvPath = vertexShaderSrcPath + ".spv";
            const std::string& fragmentShaderSpirvPath = fragmentShaderSrcPath + ".spv";

          //RIO_LOG("\n");
          //RIO_LOG("%s\n", vertexShaderSpirvPath.c_str());
          //RIO_LOG("%s\n", fragmentShaderSpirvPath.c_str());

            WriteFile(vertexShaderPath.c_str(), (u8*)vertexShaderBuf, vertexShaderBufSize);
            WriteFile(fragmentShaderPath.c_str(), (u8*)pixelShaderBuf, pixelShaderBufSize);

            std::string cmd;
            {
                std::ostringstream cmdStrm;
                cmdStrm << "\"" << ShaderUtil::sGx2ShaderDecompilerPath << "\" -v \"" << vertexShaderPath << "\" -p \"" << fragmentShaderPath << "\"";
                cmd = cmdStrm.str();
            }
          //RIO_LOG("%s\n", cmd.c_str());
            RunCommand(cmd.c_str());

            DeleteFile(vertexShaderPath.c_str());
            DeleteFile(fragmentShaderPath.c_str());

            if (!FileExists(vertexShaderSpirvPath.c_str()))
                return false;

            if (!FileExists(fragmentShaderSpirvPath.c_str()))
                return false;

            {
                std::ostringstream cmdStrm;
                cmdStrm << "\"" << ShaderUtil::sSpirvCrossPath << "\" \"" << vertexShaderSpirvPath << "\" --no-es "  \
                    "--no-420pack-extension --no-support-nonzero-baseinstance " \
                    "--version 410 --output \"" << vertexShaderSrcPath << "\"";
                cmd = cmdStrm.str();
            }
          //RIO_LOG("%s\n", cmd.c_str());
            RunCommand(cmd.c_str());

            DeleteFile(vertexShaderSpirvPath.c_str());
            if (!FileExists(vertexShaderSrcPath.c_str()))
                return false;

            {
                std::ostringstream cmdStrm;
                cmdStrm << "\"" << ShaderUtil::sSpirvCrossPath << "\" \"" << fragmentShaderSpirvPath << "\" --no-es "  \
                    "--no-420pack-extension --no-support-nonzero-baseinstance " \
                    "--version 410 --output \"" << fragmentShaderSrcPath << "\"";
                cmd = cmdStrm.str();
            }
          //RIO_LOG("%s\n", cmd.c_str());
            RunCommand(cmd.c_str());

            DeleteFile(fragmentShaderSpirvPath.c_str());
            if (!FileExists(fragmentShaderSrcPath.c_str()))
                return false;

            {
                [[maybe_unused]] bool read = ReadFile(vertexShaderSrcPath, &glVertexShader);
                RIO_ASSERT(read);
            }
            DeleteFile(vertexShaderSrcPath.c_str());

            {
                [[maybe_unused]] bool read = ReadFile(fragmentShaderSrcPath, &glFragmentShader);
                RIO_ASSERT(read);
            }
            DeleteFile(fragmentShaderSrcPath.c_str());

            ReplaceString(glVertexShader, "\r\n", "\n");
            ReplaceString(glFragmentShader, "\r\n", "\n");

            std::vector<GX2UniformBlock> vertexUBOs = std::vector<GX2UniformBlock>(vertexShader->uniformBlocks,
                                                                                   vertexShader->uniformBlocks + vertexShader->numUniformBlocks);

            std::sort(vertexUBOs.begin(), vertexUBOs.end(), GX2UniformBlockComp);

            for (u32 i = 0; i < vertexShader->numUniformBlocks; i++)
            {
                std::ostringstream formatOldStrm;
                formatOldStrm << "layout(std430) readonly buffer CBUFFER_DATA_" << vertexUBOs[i].location << std::endl
                              << "{" << std::endl
                              << "    vec4 values[];" << std::endl
                              << "}";

                std::ostringstream formatNewStrm;
                formatNewStrm << "layout(std140) uniform " << vertexUBOs[i].name << std::endl
                              << "{" << std::endl
                              << "    vec4 values[" << ((vertexUBOs[i].size + 15) / 16) << "];" << std::endl
                              << "}";

                ReplaceString(glVertexShader, formatOldStrm.str(), formatNewStrm.str());
            }

            std::vector<GX2UniformBlock> pixelUBOs = std::vector<GX2UniformBlock>(pixelShader->uniformBlocks,
                                                                                  pixelShader->uniformBlocks + pixelShader->numUniformBlocks);

            std::sort(pixelUBOs.begin(), pixelUBOs.end(), GX2UniformBlockComp);

            for (u32 i = 0; i < pixelShader->numUniformBlocks; i++)
            {
                std::ostringstream formatOldStrm;
                formatOldStrm << "layout(std430) readonly buffer CBUFFER_DATA_" << pixelUBOs[i].location << std::endl
                              << "{" << std::endl
                              << "    vec4 values[];" << std::endl
                              << "}";

                std::ostringstream formatNewStrm;
                formatNewStrm << "layout(std140) uniform " << pixelUBOs[i].name << std::endl
                              << "{" << std::endl
                              << "    vec4 values[" << ((pixelUBOs[i].size + 15) / 16) << "];" << std::endl
                              << "}";

                ReplaceString(glFragmentShader, formatOldStrm.str(), formatNewStrm.str());
            }

            for (u32 i = 0; i < vertexShader->numSamplers; i++)
            {
                const GX2SamplerVar& sampler = vertexShader->samplerVars[i];
                if (sampler.type != GX2_SAMPLER_TYPE_2D)
                    continue;

                // Remove dummy 2D samplers definitions
                {
                    std::ostringstream formatOldStrm;
                    formatOldStrm << "uniform sampler2D SPIRV_Cross_CombinedTEXTURE_"
                                  << sampler.location
                                  << "SPIRV_Cross_DummySampler;"
                                  << std::endl;

                    ReplaceString(glVertexShader, formatOldStrm.str(), "");
                }
                // Replace dummy 2D samplers
                {
                    std::ostringstream formatOldStrm;
                    formatOldStrm << "SPIRV_Cross_CombinedTEXTURE_"
                                  << sampler.location
                                  << "SPIRV_Cross_DummySampler";

                    ReplaceString(glVertexShader, formatOldStrm.str(), sampler.name);
                }
                // Replace 2D samplers
                {
                    std::ostringstream formatOldStrm;
                    formatOldStrm << "SPIRV_Cross_CombinedTEXTURE_"
                                  << sampler.location
                                  << "SAMPLER_"
                                  << sampler.location;

                    ReplaceString(glVertexShader, formatOldStrm.str(), sampler.name);
                }
            }

            for (u32 i = 0; i < pixelShader->numSamplers; i++)
            {
                const GX2SamplerVar& sampler = pixelShader->samplerVars[i];
                if (sampler.type != GX2_SAMPLER_TYPE_2D)
                    continue;

                // Remove dummy 2D samplers definitions
                {
                    std::ostringstream formatOldStrm;
                    formatOldStrm << "uniform sampler2D SPIRV_Cross_CombinedTEXTURE_"
                                  << sampler.location
                                  << "SPIRV_Cross_DummySampler;"
                                  << std::endl;

                    ReplaceString(glFragmentShader, formatOldStrm.str(), "");
                }
                // Replace dummy 2D samplers
                {
                    std::ostringstream formatOldStrm;
                    formatOldStrm << "SPIRV_Cross_CombinedTEXTURE_"
                                  << sampler.location
                                  << "SPIRV_Cross_DummySampler";

                    ReplaceString(glFragmentShader, formatOldStrm.str(), sampler.name);
                }
                // Replace 2D samplers
                {
                    std::ostringstream formatOldStrm;
                    formatOldStrm << "SPIRV_Cross_CombinedTEXTURE_"
                                  << sampler.location
                                  << "SAMPLER_"
                                  << sampler.location;

                    ReplaceString(glFragmentShader, formatOldStrm.str(), sampler.name);
                }
            }

            WriteFile(vertexShaderSrcPath, glVertexShader);
            WriteFile(fragmentShaderSrcPath, glFragmentShader);
        }

        sShaderCache.insert(ShaderCacheMap::value_type(key, ShaderCache(glVertexShader, glFragmentShader)));
    }

    rio::MemUtil::free(vertexShaderBuf);
    rio::MemUtil::free(pixelShaderBuf);
    delete[] shaderBuf;

    *vertexShaderSrc = glVertexShader;
    *fragmentShaderSrc = glFragmentShader;
    return true;
}

}

std::string ShaderUtil::sGshCompilePath = "";
std::string ShaderUtil::sTempPath = "";
std::string ShaderUtil::sGx2ShaderDecompilerPath = "";
std::string ShaderUtil::sSpirvCrossPath = "";

bool ShaderUtil::compileSource(
    const std::string& out_gsh_fname,
    const std::string& vert_fname, const std::string& frag_fname,
    const std::string& geom_fname
)
{
    if (sGshCompilePath.empty() ||
        !FileExists(sGshCompilePath.c_str()))
    {
        return false;
    }

    if (out_gsh_fname.empty() ||
        vert_fname.empty() ||
        frag_fname.empty())
    {
        return false;
    }

    bool has_geom = !geom_fname.empty();

    std::string out_gsh_native_fname;
    std::string vert_native_fname;
    std::string frag_native_fname;
    std::string geom_native_fname;

    // GSH
    {
        std::string out_gsh_fname_no_drive;

        rio::FileDevice* device = rio::FileDeviceMgr::instance()->findDeviceFromPath(out_gsh_fname, &out_gsh_fname_no_drive);
        if (!device)
            return false;

        out_gsh_native_fname = device->getNativePath(out_gsh_fname_no_drive);
      //RIO_LOG("Output: %s\n", out_gsh_native_fname.c_str());
        if (FileExists(out_gsh_native_fname.c_str()))
            DeleteFile(out_gsh_native_fname.c_str());
    }
    // Vertex
    {
        std::string vert_fname_no_drive;

        rio::FileDevice* device = rio::FileDeviceMgr::instance()->findDeviceFromPath(vert_fname, &vert_fname_no_drive);
        if (!device)
            return false;

        vert_native_fname = device->getNativePath(vert_fname_no_drive);
      //RIO_LOG("Vertex shader: %s\n", vert_native_fname.c_str());
        if (!FileExists(vert_native_fname.c_str()))
            return false;
    }
    // Fragment
    {
        std::string frag_fname_no_drive;

        rio::FileDevice* device = rio::FileDeviceMgr::instance()->findDeviceFromPath(frag_fname, &frag_fname_no_drive);
        if (!device)
            return false;

        frag_native_fname = device->getNativePath(frag_fname_no_drive);
      //RIO_LOG("Fragment shader: %s\n", frag_native_fname.c_str());
        if (!FileExists(frag_native_fname.c_str()))
            return false;
    }
    // Geometry
    if (has_geom)
    {
        std::string geom_fname_no_drive;

        rio::FileDevice* device = rio::FileDeviceMgr::instance()->findDeviceFromPath(geom_fname, &geom_fname_no_drive);
        if (!device)
            has_geom = false;
        else
        {
            geom_native_fname = device->getNativePath(geom_fname_no_drive);
          //RIO_LOG("Geometry shader: %s\n", geom_native_fname.c_str());
            if (!FileExists(geom_native_fname.c_str()))
                has_geom = false;
        }
    }

    std::string cmd;
    {
        std::ostringstream cmdStrm;
        cmdStrm << "\"" << sGshCompilePath << "\" -no_limit_array_syms -nospark -O -v \"" << vert_native_fname << "\" -p \"" << frag_native_fname << "\"";
        if (has_geom)
            cmdStrm << " -g \"" << geom_native_fname << "\"";
        cmdStrm << " -o \"" << out_gsh_native_fname << "\"";
        cmd = cmdStrm.str();
    }
  //RIO_LOG("%s\n", cmd.c_str());
    RunCommand(cmd.c_str());
    return FileExists(out_gsh_native_fname.c_str());
}

bool ShaderUtil::decompileGsh(
    const std::string& gsh_fname,
    const std::string& out_vert_fname, const std::string& out_frag_fname,
    GFDFile* p_out_gsh
)
{
    if (sTempPath.empty() ||
        !FolderExists(sTempPath.c_str()))
    {
        return false;
    }

    if (sGx2ShaderDecompilerPath.empty() ||
        !FileExists(sGx2ShaderDecompilerPath.c_str()))
    {
        return false;
    }

    if (sSpirvCrossPath.empty() ||
        !FileExists(sSpirvCrossPath.c_str()))
    {
        return false;
    }

    if (gsh_fname.empty() ||
        out_vert_fname.empty() ||
        out_frag_fname.empty())
    {
        return false;
    }

    std::string gsh_native_fname;
    std::string out_vert_native_fname;
    std::string out_frag_native_fname;

    // GSH
    {
        std::string gsh_fname_no_drive;

        rio::FileDevice* device = rio::FileDeviceMgr::instance()->findDeviceFromPath(gsh_fname, &gsh_fname_no_drive);
        if (!device)
            return false;

        gsh_native_fname = device->getNativePath(gsh_fname_no_drive);
      //RIO_LOG("Output: %s\n", gsh_native_fname.c_str());
        if (!FileExists(gsh_native_fname.c_str()))
            return false;
    }
    // Vertex
    {
        std::string out_vert_fname_no_drive;

        rio::FileDevice* device = rio::FileDeviceMgr::instance()->findDeviceFromPath(out_vert_fname, &out_vert_fname_no_drive);
        if (!device)
            return false;

        out_vert_native_fname = device->getNativePath(out_vert_fname_no_drive);
      //RIO_LOG("Vertex shader: %s\n", out_vert_native_fname.c_str());
        if (FileExists(out_vert_native_fname.c_str()))
            DeleteFile(out_vert_native_fname.c_str());
    }
    // Fragment
    {
        std::string out_frag_fname_no_drive;

        rio::FileDevice* device = rio::FileDeviceMgr::instance()->findDeviceFromPath(out_frag_fname, &out_frag_fname_no_drive);
        if (!device)
            return false;

        out_frag_native_fname = device->getNativePath(out_frag_fname_no_drive);
      //RIO_LOG("Fragment shader: %s\n", out_frag_native_fname.c_str());
        if (FileExists(out_frag_native_fname.c_str()))
            DeleteFile(out_frag_native_fname.c_str());
    }

    u8* binary = nullptr;
    if (!ReadFile(gsh_native_fname.c_str(), &binary, nullptr) || !binary)
        return false;

    GFDFile gfd;
    if (!p_out_gsh)
        p_out_gsh = &gfd;

    bool gfd_ret = p_out_gsh->load(binary);
    FreeFile(binary);
    if (!gfd_ret)
        return false;

    GX2VertexShader* const p_vert_shader = &p_out_gsh->mVertexShaders[0];
    GX2PixelShader*  const p_frag_shader = &p_out_gsh->mPixelShaders[0];

    RIO_ASSERT(p_vert_shader != nullptr);
    RIO_ASSERT(p_frag_shader != nullptr);

    std::string vert_src;
    std::string frag_src;
    if (!DecompileProgram(
            p_vert_shader, p_frag_shader,
            &vert_src, &frag_src
    ))
        return false;

    if (!WriteFile(out_vert_native_fname, vert_src))
        return false;

    if (!WriteFile(out_frag_native_fname, frag_src))
        return false;

    return FileExists(out_vert_native_fname.c_str()) &&
           FileExists(out_frag_native_fname.c_str());
}