#pragma once

#include <common/aglResCommon.h>

namespace agl {

struct ResShaderMacroData
{
    u32 mSize;
    u32 mNameLen;
    u32 mValueLen;
    // char mName[];
};
static_assert(sizeof(ResShaderMacroData) == 0xC, "agl::ResShaderMacroData size mismatch");

class ResShaderMacro : public ResCommon<ResShaderMacroData>
{
    AGL_RES_COMMON(ResShaderMacro)

public:
    const char* getName() const
    {
        return (const char*)(ptr() + 1);
    }

    u32 getNameLength() const
    {
        const char* const name = getName();
        u32 length = ptr()->mNameLen;

        while (length > 0 && name[length - 1] == '\0')
            length--;

        return length;
    }

    const char* getValue() const
    {
        return getName() + ptr()->mNameLen;
    }

    u32 getValueLength() const
    {
        const char* const value = getValue();
        u32 length = ptr()->mValueLen;

        while (length > 0 && value[length - 1] == '\0')
            length--;

        return length;
    }
};

typedef ResArray<ResShaderMacro> ResShaderMacroArray;

typedef ResShaderMacroArray::DataType ResShaderMacroArrayData;
static_assert(sizeof(ResShaderMacroArrayData) == 8, "agl::ResShaderMacroArrayData size mismatch");

}
