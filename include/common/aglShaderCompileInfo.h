#pragma once

#include <common/aglShaderEnum.h>
#include <container/Buffer.h>
#include <container/PtrArray.h>
#include <misc/Namable.h>

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
    virtual ~ShaderCompileInfo();

    void create(s32 num_macro, s32 num_variation);

    void clearMacro()
    {
        mMacroName.clear();
        mMacroValue.clear();
    }

    void pushBackMacro(const char* name, const char* value)
    {
        mMacroName.pushBack(name);
        mMacroValue.pushBack(value);
    }

    void clearVariation();
    void pushBackVariation(const char* name, const char* value);

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

    void destroy();

private:
    const std::string* mSourceText;
    std::string* mRawText;
    PtrArray<const char> mMacroName;
    PtrArray<const char> mMacroValue;
    PtrArray<const char> mVariationName;
    PtrArray<const char> mVariationValue;
    Buffer<u8> _40; // No idea buffer of what
    Buffer<u8> _48; // ^^
};
static_assert(sizeof(ShaderCompileInfo) == 0x54, "agl::ShaderCompileInfo size mismatch");

}
