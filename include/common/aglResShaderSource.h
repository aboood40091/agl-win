#pragma once

#include <common/aglResCommon.h>

namespace agl {

struct ResShaderSourceData
{
    u32 mSize;
    u32 mNameLen;
    u32 mTextLen;
    u32 mRawTextLen;    // If file includes other files, this is the length of text after
                        // resolving all includes + macros auto defined by AGL.
                        // Otherwise, it's the same as the previous field.
                        // (Unused)
    // char mName[];
};
static_assert(sizeof(ResShaderSourceData) == 0x10, "agl::ResShaderSourceData size mismatch");

class ResShaderSource : public ResCommon<ResShaderSourceData>
{
    AGL_RES_COMMON(ResShaderSource)

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

    const char* getText() const
    {
        return getName() + ptr()->mNameLen;
    }

    u32 getTextLength() const
    {
        const char* const text = getText();
        u32 length = ptr()->mTextLen;

        while (length > 0 && text[length - 1] == '\0')
            length--;

        return length;
    }
};

typedef ResArray<ResShaderSource> ResShaderSourceArray;

typedef ResShaderSourceArray::DataType ResShaderSourceArrayData;
static_assert(sizeof(ResShaderSourceArrayData) == 8, "agl::ResShaderSourceArrayData size mismatch");

}
