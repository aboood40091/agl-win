#pragma once

#include <common/aglShaderEnum.h>
#include <misc/Namable.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace agl {

class ShaderCompileInfo : public INamable
{
public:
    enum Target
    {
        cTarget_GL,
        cTarget_GX2,
        cTarget_Num
    };

public:
    ShaderCompileInfo();

    void clearMacro()
    {
        mMacroMap.clear();
    }

    void pushBackMacro(const std::string& name, const std::string& value)
    {
        [[maybe_unused]] const auto& itr = mMacroMap.try_emplace(name, value);
        RIO_ASSERT(itr.second);
    }

    void clearVariation();
    void pushBackVariation(const std::string& name, const std::string& value);

    void setSourceText(const std::string* text)
    {
        mSourceText = text;
    }

    const std::string* getSourceText() const
    {
        return mSourceText;
    }

    void setRawText(std::string* text)
    {
        mRawText = text;
    }

    std::string* getRawText() const
    {
        return mRawText;
    }

    void calcCompileSource(ShaderType type, std::string* p_buffer, Target target, bool) const;

private:
    const std::string* mSourceText;
    std::string* mRawText;
    std::unordered_map<std::string, const std::string> mMacroMap;
    std::unordered_map<std::string, const std::string> mVariationMap;
};

}
