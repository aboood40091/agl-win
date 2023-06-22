#include <common/aglShaderCompileInfo.h>
#include <common/aglShaderEnum.h>
#include <detail/aglShaderTextUtil.h>

#include <cstring>

#include "aglShaderCompileInfo_ShaderReplacements.hpp"

namespace agl {

ShaderCompileInfo::ShaderCompileInfo()
    : INamable("unititled")
    , mSourceText(nullptr)
    , mRawText(nullptr)
{
}

void ShaderCompileInfo::clearVariation()
{
    mVariationMap.clear();
}

void ShaderCompileInfo::pushBackVariation(const std::string& name, const std::string& value)
{
    [[maybe_unused]] const auto& itr = mVariationMap.try_emplace(name, value);
    RIO_ASSERT(itr.second);
}

void ShaderCompileInfo::calcCompileSource(ShaderType type, std::string* p_buffer, Target target, bool) const
{
    RIO_ASSERT(p_buffer != nullptr);

    p_buffer->clear();

    if (!mSourceText)
        return;

    const std::string* p_text = nullptr;
    for (u32 i = 0; i < STRING_REPLACEMENT_COUNT; i++)
    {
        const std::string& search_str = cStringReplacement_From[i];
        if (search_str == *mSourceText)
        {
            p_text = &(cStringReplacement_To[i]);
            break;
        }
    }

    if (p_text == nullptr)
        p_text = mSourceText;

    const std::string& text = *p_text;
    std::string::size_type text_start_pos = 0;

    bool is_gl = target == cTarget_GL;


    const std::string::size_type version_pos = text.find("#version", 0, 8);
    if (version_pos != std::string::npos)
    {
        const std::string::size_type line_feed_pos = text.find_first_of("\r\n", version_pos, 2);
        if (line_feed_pos != std::string::npos)
            text_start_pos = line_feed_pos + 1 + u32(text[line_feed_pos] == '\r' && text[line_feed_pos + 1] == '\n');
    }

    static const std::string sMacroDefine[] = {
        // GL
        "#version 400\n" \
        "#extension GL_ARB_texture_cube_map_array : enable\n" \
        "#extension GL_ARB_shading_language_420pack : enable\n",

        // GX2
        "#version 330\n" \
        "#extension GL_ARB_texture_cube_map_array : enable\n"
    };

    static const std::string sTargetDefine[] = {
        "#define AGL_TARGET_GL \n",
        "#define AGL_TARGET_GX2 \n"
    };

    static const std::string sTypeDefine[cShaderType_Num] = {
        "#define AGL_VERTEX_SHADER \n",
        "#define AGL_FRAGMENT_SHADER \n",
        "#define AGL_GEOMETRY_SHADER \n"
    };

    static const std::string cComments[] = {
        "// ----- These macros are auto defined by AGL.-----\n",
        "// ------------------------------------------------\n"
    };

    p_buffer->append(sMacroDefine[is_gl ? 0 : 1]);
    p_buffer->append(cComments[0]);
    p_buffer->append(sTypeDefine[type]);
    p_buffer->append(sTargetDefine[is_gl ? 0 : 1]);
    p_buffer->append(cComments[1]);
    p_buffer->append(text, text_start_pos);

    if (mMacroMap.size() > 0)
    {
        detail::ShaderTextUtil::replaceMacro(
            p_buffer,
            mMacroMap
        );
    }

    if (mVariationMap.size() > 0)
    {
        detail::ShaderTextUtil::replaceMacro(
            p_buffer,
            mVariationMap
        );
    }

    if (mRawText)
        mRawText->assign(*p_buffer);
}

}
