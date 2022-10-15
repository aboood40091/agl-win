#pragma once

#include <misc/rio_Types.h>

#include <string>

class GFDFile;

class ShaderUtil
{
public:
    static std::string sGshCompilePath;
    static std::string sTempPath;
    static std::string sGx2ShaderDecompilerPath;
    static std::string sSpirvCrossPath;

public:
    static bool compileSource(
        const std::string& out_gsh_fname,
        const std::string& vert_fname, const std::string& frag_fname,
        const std::string& geom_fname
    );

    static bool decompileGsh(
        const std::string& gsh_fname,
        const std::string& out_vert_fname, const std::string& out_frag_fname,
      //const std::string& out_geom_fname, // <-- TODO
        GFDFile* p_out_gsh = nullptr
    );
};
