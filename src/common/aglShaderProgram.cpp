#include <common/aglDisplayList.h>
#include <common/aglShaderCompileInfo.h>
#include <common/aglShaderProgram.h>
#include <detail/aglFileIOMgr.h>
#include <detail/aglGX2.h>
#include <detail/aglPrivateResource.h>
//#include <gfx/seadGraphics.h>

#include <cstring>

namespace agl {

void ShaderProgram::changeShaderMode(ShaderMode mode)
{
    driver::GX2Resource::instance()->setShaderMode(mode);
}

ShaderProgram::ShaderProgram()
    : mFlag(0)
    , mVariationID(0)
    , mDisplayList()
    , mAttributeLocation()
    , mUniformLocation()
    , mUniformBlockLocation()
    , mSamplerLocation()
    , mVertexShader()
    , mFragmentShader()
    , mGeometryShader()
    , mpSharedData(nullptr)
{
}

ShaderProgram::~ShaderProgram()
{
    cleanUp();

    destroyAttribute();
    destroyUniform();
    destroySamplerLocation();
    destroyUniformBlock();

    if (mpSharedData->mpOriginal == this)
    {
        delete mpSharedData->mpVariationBuffer;
        delete mpSharedData;
    }
}

void ShaderProgram::initialize(const char* name)
{
    mpSharedData = new SharedData();
    mpSharedData->mpOriginal = this;
    mpSharedData->mpVariationBuffer = nullptr;
    mpSharedData->_10 = 0;
    mpSharedData->setName(name);
    mpSharedData->_28 = 0;

    for (UnsafeArray<ResShaderSymbolArray, cShaderSymbolType_Num>::iterator it = mpSharedData->mResShaderSymbolArray.begin(), it_end = mpSharedData->mResShaderSymbolArray.end(); it != it_end; ++it)
        *it = nullptr;
}

void ShaderProgram::createVariationBuffer(s32 macro_num)
{
    mpSharedData->mpVariationBuffer = new VariationBuffer();
    mpSharedData->mpVariationBuffer->initialize(this, macro_num);
}

void ShaderProgram::createVariationMacro(s32 index, const char* name, const char* id, s32 value_num)
{
    mpSharedData->mpVariationBuffer->createMacro(index, name, id, value_num);
}

void ShaderProgram::setVariationMacroValue(s32 macro_index, s32 value_index, const char* value)
{
    mpSharedData->mpVariationBuffer->setMacroValue(macro_index, value_index, value);
}

void ShaderProgram::createVariation()
{
    mpSharedData->mpVariationBuffer->create();

    for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
    {
        it->mpSharedData = mpSharedData;
        it->mVariationID = it.getIndex() + 1;
    }
}

ShaderMode ShaderProgram::activate(ShaderMode current_mode, bool use_dl) const
{
#if RIO_IS_CAFE
    ShaderMode mode = getGeometryShaderBinary()
        ? cShaderMode_GeometryShader
        : mVertexShader.getShaderMode();

    if (current_mode != mode)
        changeShaderMode(mode);

    current_mode = mode;

    if (current_mode == cShaderMode_GeometryShader)
        driver::GX2Resource::instance()->setGeometryShaderRingBuffer();
#endif // RIO_IS_CAFE

    if (use_dl && !mDisplayList.isEmpty())
        mDisplayList.call();
    else
        setShaderGX2_();

    return current_mode;
}

Shader* ShaderProgram::getShader(ShaderType type)
{
    switch (type)
    {
    case cShaderType_Vertex:
        return &mVertexShader;
    case cShaderType_Fragment:
        return &mFragmentShader;
    case cShaderType_Geometry:
        return &mGeometryShader;
    default:
        return nullptr;
    }
}

const Shader* ShaderProgram::getShader(ShaderType type) const
{
    switch (type)
    {
    case cShaderType_Vertex:
        return &mVertexShader;
    case cShaderType_Fragment:
        return &mFragmentShader;
    case cShaderType_Geometry:
        return &mGeometryShader;
    default:
        return nullptr;
    }
}

void ShaderProgram::createAttribute(s32 num)
{
    RIO_ASSERT(mAttributeLocation.size() == 0);
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mAttributeLocation.allocBuffer(num);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mAttributeLocation.allocBuffer(num);
    }
}

void ShaderProgram::setAttributeName(s32 index, const char* name)
{
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mAttributeLocation[index].setName(name);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mAttributeLocation[index].setName(name);
    }
}

void ShaderProgram::createUniform(s32 num)
{
    RIO_ASSERT(mUniformLocation.size() == 0);
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mUniformLocation.allocBuffer(num);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mUniformLocation.allocBuffer(num);
    }
}

void ShaderProgram::setUniformName(s32 index, const char* name)
{
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mUniformLocation[index].setName(name);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mUniformLocation[index].setName(name);
    }
}

void ShaderProgram::createUniformBlock(s32 num)
{
    RIO_ASSERT(mUniformBlockLocation.size() == 0);
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mUniformBlockLocation.allocBuffer(num);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mUniformBlockLocation.allocBuffer(num);
    }
}

void ShaderProgram::setUniformBlockName(s32 index, const char* name)
{
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mUniformBlockLocation[index].setName(name);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mUniformBlockLocation[index].setName(name);
    }
}

void ShaderProgram::createSamplerLocation(s32 num)
{
    RIO_ASSERT(mSamplerLocation.size() == 0);
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mSamplerLocation.allocBuffer(num);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mSamplerLocation.allocBuffer(num);
    }
}

void ShaderProgram::setSamplerLocationName(s32 index, const char* name)
{
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mSamplerLocation[index].setName(name);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mSamplerLocation[index].setName(name);
    }
}

u32 ShaderProgram::setUpAllVariation()
{
    u32 ret;

    if (getVariation_())
    {
        ret = getVariation_()->mpOriginal->validate_();
        if (ret == 0)
        {
            for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            {
                ret = it->validate_();
                if (ret != 0)
                    break;
            }
        }
    }
    else
    {
        ret = validate_();
    }

    return ret;
}

void ShaderProgram::reserveSetUpAllVariation()
{
    if (getVariation_())
    {
        getVariation_()->mpOriginal->mFlag.set(2);
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mFlag.set(2);
    }
    else
    {
        mFlag.set(2);
    }
}

s32 ShaderProgram::getVariationNum() const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return 1;

    return variation_buffer->mProgram.size() + 1;
}

s32 ShaderProgram::getVariationMacroNum() const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return 0;

    return variation_buffer->mMacroData.size();
}

s32 ShaderProgram::searchVariationShaderProgramIndex(s32 macro_num, const char* const* macro_array, const char* const* value_array) const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return 0;

    return variation_buffer->searchShaderProgramIndex(macro_num, macro_array, value_array, mVariationID);
}

ShaderProgram* ShaderProgram::getVariation(s32 index)
{
    VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return this;

    if (index <= 0)
        return variation_buffer->mpOriginal;

    return &variation_buffer->mProgram[index - 1];
}

const ShaderProgram* ShaderProgram::getVariation(s32 index) const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return this;

    if (index <= 0)
        return variation_buffer->mpOriginal;

    return &variation_buffer->mProgram[index - 1];
}

const char* ShaderProgram::searchVariationMacroName(const char* id) const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return "";

    return variation_buffer->searchMacroName(id);
}

s32 ShaderProgram::getVariationMacroValueVariationNum(s32 macro_index) const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return 0;

    return variation_buffer->getMacroValueVariationNum(macro_index);
}

void ShaderProgram::updateAttributeLocation() const
{
    for (Buffer<AttributeLocation>::iterator it = mAttributeLocation.begin(), it_end = mAttributeLocation.end(); it != it_end; ++it)
        it->search(*this);
}

void ShaderProgram::updateUniformLocation() const
{
    for (Buffer<UniformLocation>::iterator it = mUniformLocation.begin(), it_end = mUniformLocation.end(); it != it_end; ++it)
        it->search(*this);
}

void ShaderProgram::updateUniformBlockLocation() const
{
    for (Buffer<UniformBlockLocation>::iterator it = mUniformBlockLocation.begin(), it_end = mUniformBlockLocation.end(); it != it_end; ++it)
        it->search(*this);
}

void ShaderProgram::updateSamplerLocation() const
{
    for (Buffer<SamplerLocation>::iterator it = mSamplerLocation.begin(), it_end = mSamplerLocation.end(); it != it_end; ++it)
        it->search(*this);
}

void ShaderProgram::dump() const
{
/*
#if RIO_IS_CAFE
    sead::FormatFixedSafeString<1024> cmd_arg(
        "File = %s, WaitEnd = True, WindowStyle = Hidden",
        "%AGL_ROOT%/tools/bin/Win32/gshCompile_tmp.bat"
    );

    // Stripped in release build:
    // sead::hostio::ShellExecuteRequest(cmd_arg, "");

    detail::FileIOMgr::DialogArg arg;

    arg.mPath = "%AGL_ROOT%/tools/temporary/temp.gsh";
    arg.mLoadAlignment = GX2_SHADER_ALIGNMENT;

    if (detail::FileIOMgr::instance()->load(arg) != -1 && // Never unloaded...
        mFlag.isOn(8))
    {
        sead::SafeString disasm_str = "; --------  Disassembly --------------------";
        sead::SafeString symbol_str = "-----------------Symbol Section ----------------------";
        s32 disasm_str_len = disasm_str.calcLength();
        s32 symbol_str_len = symbol_str.calcLength();

        sead::Heap* heap = detail::PrivateResource::instance()->getShaderCompileHeap();

        sead::HeapSafeString* analyze_str = new (heap) sead::HeapSafeString(heap, "", 0x100000);

        static const char* sTempFname[cShaderType_Num] = {
            "%AGL_ROOT%/tools/temporary/temp_vs.dmp",
            "%AGL_ROOT%/tools/temporary/temp_ps.dmp",
            "%AGL_ROOT%/tools/temporary/temp_gs.dmp"
        };

        static const char* sShaderType[cShaderType_Num] = {
            "Vertex Shader",
            "Fragment Shader",
            "Geometry Shader"
        };

        for (s32 type = 0; type < cShaderType_Num; type++)
        {
            arg.mPath = sTempFname[type];
            s32 index = detail::FileIOMgr::instance()->load(arg);
            if (index != -1)
            {
                const char* text = (char*)detail::FileIOMgr::instance()->getFile(index).mpData;
                while (*text != '\0')
                {
                    // TODO: Too much is happening here
                    ++text;
                    (void)sShaderType;
                }
                detail::FileIOMgr::instance()->close(index);
            }
        }

        arg.mPath = sead::FormatFixedSafeString<1024>("%%AGL_ROOT%%/tools/temporary/shader_analyze/%s.txt", getName().cstr());
        detail::FileIOMgr::instance()->save(analyze_str->cstr(), analyze_str->calcLength(), arg);

        delete analyze_str;
    }
#endif // RIO_IS_CAFE
*/
}

u32 ShaderProgram::validate_() const
{
    if (mFlag.isOn(2))
    {
        mFlag.reset(2);
        return forceValidate_();
    }

    return 0;
}

u32 ShaderProgram::forceValidate_() const
{
    u32 ret = 0;
    bool compile_source = mFlag.isOn(1);

    setUpForVariation_();

    if (mVertexShader.setUp(compile_source, mFlag.isOn(8)) != 0)
    {
        ret = 1;
    }
    else if (mFragmentShader.setUp(compile_source, mFlag.isOn(8)) != 0)
    {
        ret = 2;
    }
    else if (mGeometryShader.setUp(compile_source, mFlag.isOn(8)) == 1)
    {
        ret = 3;
    }
    else
    {
        if (mFlag.isOn(1))
        {
#if RIO_IS_WIN
            mShader.unload();

            const ShaderCompileInfo* p_vert_compile_info = mVertexShader.getCompileInfo();
            RIO_ASSERT(p_vert_compile_info != nullptr);

            const std::string* p_vert_src = p_vert_compile_info->getRawText();
            RIO_ASSERT(p_vert_src != nullptr);

            const ShaderCompileInfo* p_frag_compile_info = mFragmentShader.getCompileInfo();
            RIO_ASSERT(p_frag_compile_info != nullptr);

            const std::string* p_frag_src = p_frag_compile_info->getRawText();
            RIO_ASSERT(p_frag_src != nullptr);

            mShader.load(p_vert_src->c_str(), p_frag_src->c_str());
#endif // RIO_IS_WIN
            dump();

            if (mDisplayList.getBuffer() != nullptr)
            {
                void* p_dl;
                size_t size = DisplayList::suspend(&p_dl);
                {
                    mDisplayList.beginDisplayList();
                    {
                        setShaderGX2_();
                    }
                    mDisplayList.endDisplayList();
                }
                DisplayList::resume(p_dl, size);
            }
        }
        else
        {
            if (mDisplayList.getBuffer() != nullptr)
            {
              //sead::Graphics::instance()->lockDrawContext();
                {
                    mDisplayList.beginDisplayList();
                    {
                        setShaderGX2_();
                    }
                    mDisplayList.endDisplayList();
                }
              //sead::Graphics::instance()->unlockDrawContext();
            }
        }

        mFlag.setDirect(1);
    }

    if (mFlag.isOn(1))
    {
        updateUniformLocation();
        updateUniformBlockLocation();
        updateAttributeLocation();
        updateSamplerLocation();
    }

    // TODO
    // if (mpSharedData->_10)
    //     mpSharedData->_10->vf0C(this);

    return ret;
}

void ShaderProgram::setUpForVariation_() const
{
    if (!getVariation_())
        return;

    const ShaderProgram* program = getVariation_()->mpOriginal;

    const char* macro_array[cVariationMacroMax];
    const char* value_array[cVariationValueMax];
    s32 num_macro_value = getVariation_()->getMacroAndValueArray(mVariationID, macro_array, value_array);

    for (s32 type = 0; type < cShaderType_Num; type++)
    {
        ShaderCompileInfo* compile_info = program->getShader(ShaderType(type))->getCompileInfo();
        if (!compile_info)
            continue;

        compile_info->clearVariation();

        for (s32 i_variation_type = 0; i_variation_type < num_macro_value; i_variation_type++)
        {
            RIO_ASSERT(i_variation_type < cVariationMacroMax);
            RIO_ASSERT(i_variation_type < cVariationValueMax);
            compile_info->pushBackVariation(macro_array[i_variation_type], value_array[i_variation_type]);
        }

        getShader(ShaderType(type))->setCompileInfo(compile_info);
    }
}

void ShaderProgram::setShaderGX2_() const
{
#if RIO_IS_CAFE
    if (mVertexShader.getBinary())
        GX2SetVertexShader(mVertexShader.getBinary());

    if (mFragmentShader.getBinary())
        GX2SetPixelShader(mFragmentShader.getBinary());

    if (mGeometryShader.getBinary())
        GX2SetGeometryShader(mGeometryShader.getBinary());
#elif RIO_IS_WIN
    mShader.bind();
#endif
}

void ShaderProgram::cleanUp()
{
    if (mFlag.isOn(1))
        mFlag.reset(1);
}

void ShaderProgram::destroyAttribute()
{
    mAttributeLocation.freeBuffer();

    VariationBuffer* variation_buffer = getVariation_();
    if (variation_buffer)
    {
        for (Buffer<ShaderProgram>::iterator it = variation_buffer->mProgram.begin(), it_end = variation_buffer->mProgram.end(); it != it_end; ++it)
            it->mAttributeLocation.freeBuffer();
    }
}

void ShaderProgram::destroyUniform()
{
    mUniformLocation.freeBuffer();

    VariationBuffer* variation_buffer = getVariation_();
    if (variation_buffer)
    {
        for (Buffer<ShaderProgram>::iterator it = variation_buffer->mProgram.begin(), it_end = variation_buffer->mProgram.end(); it != it_end; ++it)
            it->mUniformLocation.freeBuffer();
    }
}

void ShaderProgram::destroyUniformBlock()
{
    mUniformBlockLocation.freeBuffer();

    VariationBuffer* variation_buffer = getVariation_();
    if (variation_buffer)
    {
        for (Buffer<ShaderProgram>::iterator it = variation_buffer->mProgram.begin(), it_end = variation_buffer->mProgram.end(); it != it_end; ++it)
            it->mUniformBlockLocation.freeBuffer();
    }
}

void ShaderProgram::destroySamplerLocation()
{
    mSamplerLocation.freeBuffer();

    VariationBuffer* variation_buffer = getVariation_();
    if (variation_buffer)
    {
        for (Buffer<ShaderProgram>::iterator it = variation_buffer->mProgram.begin(), it_end = variation_buffer->mProgram.end(); it != it_end; ++it)
            it->mSamplerLocation.freeBuffer();
    }
}

ShaderProgram::VariationBuffer::VariationBuffer()
    : mProgram()
    , mMacroData()
{
}

ShaderProgram::VariationBuffer::~VariationBuffer()
{
    mProgram.freeBuffer();

    for (Buffer<MacroData>::iterator it = mMacroData.begin(), it_end = mMacroData.end(); it != it_end; ++it)
        it->mValue.freeBuffer();

    mMacroData.freeBuffer();
}

void ShaderProgram::VariationBuffer::initialize(ShaderProgram* program, s32 macro_num)
{
    mpOriginal = program;
    mMacroData.allocBuffer(macro_num);
}

void ShaderProgram::VariationBuffer::createMacro(s32 index, const char* name, const char* id, s32 value_num)
{
    MacroData& macro = mMacroData[index];
    macro.mName = name;
    macro.mID = id;
    macro.mValueVariationNum = 1;
    macro.mValue.allocBuffer(value_num);
}

void ShaderProgram::VariationBuffer::setMacroValue(s32 macro_index, s32 value_index, const char* value)
{
    mMacroData[macro_index].mValue[value_index] = value;
}

s32 ShaderProgram::VariationBuffer::searchShaderProgramIndex(s32 macro_num, const char* const* macro_array, const char* const* value_array, s32 index) const
{
    s32 value_index_array[cVariationValueMax];

    if (index == -1)
    {
        for (Buffer<MacroData>::constIterator itr_type = mMacroData.begin(), it_end = mMacroData.end(); itr_type != it_end; ++itr_type)
            value_index_array[itr_type.getIndex()] = 0;
    }
    else
    {
        getMacroValueIndexArray(index, value_index_array);
    }

    for (Buffer<MacroData>::constIterator itr_type = mMacroData.begin(), it_end = mMacroData.end(); itr_type != it_end; ++itr_type)
    {
        for (s32 idx_macro = 0; idx_macro < macro_num; idx_macro++)
        {
            const char* macro_name = macro_array[idx_macro];
            if (std::strcmp(itr_type->mName, macro_name) == 0)
            {
                const char* macro_value = value_array[idx_macro];
                for (Buffer<const char*>::constIterator itr_value = itr_type->mValue.begin(), value_it_end = itr_type->mValue.end(); itr_value != value_it_end; ++itr_value)
                {
                    if (std::strcmp(*itr_value, macro_value) == 0)
                    {
                        value_index_array[itr_type.getIndex()] = itr_value.getIndex();
                        break;
                    }
                }
                break;
            }
        }
    }

    return calcVariationIndex(value_index_array);
}

const char* ShaderProgram::VariationBuffer::searchMacroName(const char* id) const
{
    for (Buffer<MacroData>::constIterator itr_type = mMacroData.begin(), it_end = mMacroData.end(); itr_type != it_end; ++itr_type)
        if (std::strcmp(id, itr_type->mID) == 0)
            return itr_type->mName;

    return "";
}

void ShaderProgram::VariationBuffer::create()
{
    s32 variation_num = 1;

    for (Buffer<MacroData>::iterator it = mMacroData.begin(), it_end = mMacroData.end(); it != it_end; ++it)
    {
        for (s32 i = it.getIndex() + 1; i < mMacroData.size(); i++)
            it->mValueVariationNum *= mMacroData[i].mValue.size();

        variation_num *= it->mValue.size();
    }

    if (variation_num > 1)
        mProgram.allocBuffer(variation_num - 1);
}

s32 ShaderProgram::VariationBuffer::getMacroAndValueArray(s32 index, const char** macro_array, const char** value_array) const
{
    RIO_ASSERT(macro_array != nullptr);
    RIO_ASSERT(value_array != nullptr);

    for (Buffer<MacroData>::constIterator itr_type = mMacroData.begin(), it_end = mMacroData.end(); itr_type != it_end; ++itr_type)
    {
        s32 value_index; // = 0;
      // if (itr_type->mValueVariationNum != 0)
            value_index = index / itr_type->mValueVariationNum;
        RIO_ASSERT(itr_type.getIndex() < cVariationMacroMax);
        RIO_ASSERT(itr_type.getIndex() < cVariationValueMax);
        macro_array[itr_type.getIndex()] = itr_type->mName;
        value_array[itr_type.getIndex()] = itr_type->mValue[value_index];

        index -= value_index * itr_type->mValueVariationNum;
    }

    return mMacroData.size();
}

s32 ShaderProgram::VariationBuffer::getMacroValueIndexArray(s32 index, s32* value_index_array) const
{
    for (Buffer<MacroData>::constIterator itr_type = mMacroData.begin(), it_end = mMacroData.end(); itr_type != it_end; ++itr_type)
    {
        s32 value_index; // = 0;
      // if (itr_type->mValueVariationNum != 0)
            value_index = index / itr_type->mValueVariationNum;

        value_index_array[itr_type.getIndex()] = value_index;

        index -= value_index * itr_type->mValueVariationNum;
    }

    return mMacroData.size();
}

s32 ShaderProgram::VariationBuffer::calcVariationIndex(const s32* value_index_array) const
{
    s32 index = 0;
    for (Buffer<MacroData>::constIterator itr_type = mMacroData.begin(), it_end = mMacroData.end(); itr_type != it_end; ++itr_type)
    {
        s32 value_index = value_index_array[itr_type.getIndex()];
        index += value_index * itr_type->mValueVariationNum;
    }
    return index;
}

}
