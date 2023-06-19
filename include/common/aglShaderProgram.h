#pragma once

#include <common/aglDisplayList.h>
#include <common/aglResShaderSymbol.h>
#include <common/aglResShaderVariation.h>
#include <common/aglShader.h>
#include <common/aglShaderLocation.h>
#include <container/Buffer.h>
#include <container/SafeArray.h>
#include <misc/rio_BitFlag.h>
#include <misc/Namable.h>

#if RIO_IS_WIN
#include <gpu/rio_Shader.h>
#include <cafe/gfd.h>
#endif // RIO_IS_WIN

namespace agl {

class ShaderProgram
{
public:
    static const s32 cVariationMacroMax = 1024;
    static const s32 cVariationValueMax = 1024;

    static void changeShaderMode(ShaderMode mode);

public:
    ShaderProgram();
    virtual ~ShaderProgram();

    s32 getVariationID() const
    {
        return mVariationID;
    }

    DisplayList& getDisplayList()
    {
        return mDisplayList;
    }

    const DisplayList& getDisplayList() const
    {
        return mDisplayList;
    }

    const char* getName() const
    {
        return mpSharedData->getName();
    }

    const ResShaderSymbolArray& getResShaderSymbolArray(ShaderSymbolType type) const
    {
        return mpSharedData->mResShaderSymbolArray[type];
    }

    void setResShaderSymbolArray(ShaderSymbolType type, const ResShaderSymbolArray& array)
    {
        mpSharedData->mResShaderSymbolArray[type] = array;
    }

    void setResShaderVariationDefaultArray(const ResShaderVariationArray& array)
    {
        mpSharedData->mResShaderVariationDefaultArray = array;
    }

    void initialize(const char* name);

    void createVariationBuffer(s32 macro_num);

    void createVariationMacro(s32 index, const char* name, const char* id, s32 value_num);
    void setVariationMacroValue(s32 macro_index, s32 value_index, const char* value);

    void createVariation();

    ShaderMode activate(ShaderMode current_mode = cShaderMode_Invalid, bool use_dl = true) const;

    Shader* getShader(ShaderType type);
    const Shader* getShader(ShaderType type) const;

#if RIO_IS_CAFE
    GX2VertexShader* getVertexShaderBinary()
    {
        validate_();
        return mVertexShader.getBinary();
    }

    const GX2VertexShader* getVertexShaderBinary() const
    {
        validate_();
        return mVertexShader.getBinary();
    }

    GX2PixelShader* getFragmentShaderBinary()
    {
        validate_();
        return mFragmentShader.getBinary();
    }

    const GX2PixelShader* getFragmentShaderBinary() const
    {
        validate_();
        return mFragmentShader.getBinary();
    }

    GX2GeometryShader* getGeometryShaderBinary()
    {
        validate_();
        return mGeometryShader.getBinary();
    }

    const GX2GeometryShader* getGeometryShaderBinary() const
    {
        validate_();
        return mGeometryShader.getBinary();
    }
#elif RIO_IS_WIN
    rio::Shader& getShaderRIO()
    {
        validate_();
        return mShader;
    }

    const rio::Shader& getShaderRIO() const
    {
        validate_();
        return mShader;
    }

    GX2VertexShader* getVertexShaderBinary()
    {
        validate_();
        return &mGFDFile.mVertexShaders[0];
    }

    const GX2VertexShader* getVertexShaderBinary() const
    {
        validate_();
        return &mGFDFile.mVertexShaders[0];
    }

    GX2PixelShader* getFragmentShaderBinary()
    {
        validate_();
        return &mGFDFile.mPixelShaders[0];
    }

    const GX2PixelShader* getFragmentShaderBinary() const
    {
        validate_();
        return &mGFDFile.mPixelShaders[0];
    }
#endif

    void createAttribute(s32 num);
    void setAttributeName(s32 index, const char* name);

    void createUniform(s32 num);
    void setUniformName(s32 index, const char* name);

    void createUniformBlock(s32 num);
    void setUniformBlockName(s32 index, const char* name);

    void createSamplerLocation(s32 num);
    void setSamplerLocationName(s32 index, const char* name);

    u32 setUpAllVariation(); // I don't know the actual return type
    void reserveSetUpAllVariation();

    s32 getVariationNum() const;
    s32 getVariationMacroNum() const;

    s32 searchVariationShaderProgramIndex(s32 macro_num, const char* const* macro_array, const char* const* value_array) const;

    ShaderProgram* getVariation(s32 index);
    const ShaderProgram* getVariation(s32 index) const;

    const ShaderProgram* searchVariationShaderProgram(s32 macro_num, const char* const* macro_array, const char* const* value_array) const
    {
        s32 index = searchVariationShaderProgramIndex(macro_num, macro_array, value_array);
        return getVariation(index);
    }

    const char* searchVariationMacroName(const char* id) const;

    s32 getVariationMacroValueVariationNum(s32 macro_index) const;

#if RIO_IS_WIN

    bool getCompileEnable() const
    {
        return mFlag.isOn(1);
    }

    void setCompileEnable(bool enable)
    {
        mFlag.change(1, enable);

        if (getVariation_())
        {
            for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
                it->mFlag.change(1, enable);
        }
    }

#endif // RIO_IS_WIN

    void update() const // Shrug
    {
        validate_();
    }

    void updateVariation(s32 index) // I don't know the actual name
    {
        ShaderProgram* program = getVariation(index);
        program->mFlag.set(8 | 2);
        program->validate_();
    }

    const AttributeLocation& getAttributeLocation(s32 index) const { return mAttributeLocation[index]; }
    const UniformLocation& getUniformLocation(s32 index) const { return mUniformLocation[index]; }
    const UniformBlockLocation& getUniformBlockLocation(s32 index) const { return mUniformBlockLocation[index]; }
    const SamplerLocation& getSamplerLocation(s32 index) const { return mSamplerLocation[index]; }

    void updateAttributeLocation() const;
    void updateUniformLocation() const;
    void updateUniformBlockLocation() const;
    void updateSamplerLocation() const;

    void dump() const;

    void cleanUp();

    void destroyAttribute();
    void destroyUniform();
    void destroyUniformBlock();
    void destroySamplerLocation();

private:
    u32 validate_() const;
    u32 forceValidate_() const;

    void setUpForVariation_() const;

    void setShaderGX2_() const;

    class SharedData;
    class VariationBuffer;

private:
    class VariationBuffer
    {
        VariationBuffer();
        virtual ~VariationBuffer();

        void initialize(ShaderProgram* program, s32 macro_num);

        void createMacro(s32 index, const char* name, const char* id, s32 value_num);
        void setMacroValue(s32 macro_index, s32 value_index, const char* value);

        s32 searchShaderProgramIndex(s32 macro_num, const char* const* macro_array, const char* const* value_array, s32 index) const;

        const char* searchMacroName(const char* id) const;

        void create();

        s32 getMacroAndValueArray(s32 index, const char** macro_array, const char** value_array) const;
        s32 getMacroValueIndexArray(s32 index, s32* value_index_array) const;
        s32 calcVariationIndex(const s32* value_index_array) const;

        s32 getMacroValueVariationNum(s32 macro_index) const
        {
            return mMacroData[macro_index].mValueVariationNum;
        }

        class MacroData
        {
            const char* mName;
            const char* mID;
            Buffer<const char*> mValue;
            u16 mValueVariationNum;                 // Number of variations using *succeeding* macros (i.e., ignoring preceding macros), if value of this macro is fixed.
                                                    // i.e., In a matrix where variations are the rows, macros are the columns, and a cell would be a macro's value
                                                    // in a certain variation, this field is the number a macro's value occurs before the next value in the macro's column.

            friend class VariationBuffer;
        };
      //static_assert(sizeof(MacroData) == 0x1C, "agl::ShaderProgram::VariationBuffer::MacroData size mismatch");

        ShaderProgram* mpOriginal;
        Buffer<ShaderProgram> mProgram;
        Buffer<MacroData> mMacroData;

        friend class ShaderProgram;
        friend class SharedData;
    };
    static_assert(sizeof(VariationBuffer) == 0x18, "agl::ShaderProgram::VariationBuffer size mismatch");

    class SharedData : public INamable
    {
        ShaderProgram* mpOriginal;
        VariationBuffer* mpVariationBuffer;
        u32 _10;
        ResShaderVariationArray mResShaderVariationDefaultArray; // Variation macro defaults
        UnsafeArray<ResShaderSymbolArray, cShaderSymbolType_Num> mResShaderSymbolArray;
        u32 _28;

        friend class ShaderProgram;
    };
    static_assert(sizeof(SharedData) == 0x2C, "agl::ShaderProgram::SharedData size mismatch");

    VariationBuffer* getVariation_() { return mpSharedData->mpVariationBuffer; }
    const VariationBuffer* getVariation_() const { return mpSharedData->mpVariationBuffer; }

private:
    mutable rio::BitFlag8 mFlag;
    u16 mVariationID;
    mutable DisplayList mDisplayList;
    mutable Buffer<AttributeLocation> mAttributeLocation;
    mutable Buffer<UniformLocation> mUniformLocation;
    mutable Buffer<UniformBlockLocation> mUniformBlockLocation;
    mutable Buffer<SamplerLocation> mSamplerLocation;
    VertexShader mVertexShader;
    FragmentShader mFragmentShader;
    GeometryShader mGeometryShader;
    SharedData* mpSharedData;

#if RIO_IS_WIN
    // Custom
    mutable rio::Shader mShader;
    mutable GFDFile mGFDFile;
    mutable s32 mVsCfileBlockIdx;
    mutable s32 mPsCfileBlockIdx;
#endif // RIO_IS_WIN
};
//static_assert(sizeof(ShaderProgram) == 0x60, "agl::ShaderProgram size mismatch");

}
