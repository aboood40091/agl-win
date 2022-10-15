#include <common/aglShaderProgramArchive.h>
#include <detail/aglPrivateResource.h>
#include <detail/aglShaderTextUtil.h>

#include <cstring>

#if RIO_IS_CAFE
#include <cafe.h>
#endif // RIO_IS_CAFE

namespace {

void createShaderProgram(
    agl::ShaderProgram* program,
    const char* name,
    const agl::ResShaderVariationArray& variation_macros,
    const agl::ResShaderVariationArray& variation_macro_defaults,
    const agl::ResShaderSymbolArray& attributes,
    const agl::ResShaderSymbolArray& uniforms,
    const agl::ResShaderSymbolArray& uniform_blocks,
    const agl::ResShaderSymbolArray& samplers
)
{
    program->initialize(name);

    program->setResShaderVariationDefaultArray(variation_macro_defaults);
    program->setResShaderSymbolArray(agl::cShaderSymbolType_Attribute, attributes);
    program->setResShaderSymbolArray(agl::cShaderSymbolType_Uniform, uniforms);
    program->setResShaderSymbolArray(agl::cShaderSymbolType_UniformBlock, uniform_blocks);
    program->setResShaderSymbolArray(agl::cShaderSymbolType_Sampler, samplers);

    u32 num_variation_macro = variation_macros.getNum();
    if (num_variation_macro == 0)
        return;

    program->createVariationBuffer(num_variation_macro);

    for (agl::ResShaderVariationArray::constIterator it = variation_macros.begin(), it_end = variation_macros.end(); it != it_end; ++it)
    {
        const agl::ResShaderVariation variation(&(*it));

        program->createVariationMacro(
            it.getIndex(),
            variation.getName(),
            variation.getID(),
            variation.ref().mValueNum
        );

        for (s32 i = 0; i < variation.ref().mValueNum; i++)
        {
            program->setVariationMacroValue(
                it.getIndex(),
                i,
                variation.getValue(i)
            );
        }
    }

    program->createVariation();
}

}

namespace agl {

ShaderProgramArchive::ShaderProgramArchive()
    : mResBinary()
    , mResText()
    , mProgram()
    , _20(0)
    , _24(0)
    , _28(0)
    , mFlag()
    , mProgramEx()
    , mSource()
    , mSourceText()
    , mSourceName()
    , mpDLBuf(nullptr)
{
    // detail::RootNode::setNodeMeta(this, "Icon = LAYOUT, Security = agl_shader");
}

ShaderProgramArchive::~ShaderProgramArchive()
{
    destroy();
}

void ShaderProgramArchive::destroy()
{
    destroyResFile_();
    mProgram.freeBuffer();
    mResBinary = NULL;
    _28 = 0;

    if (mpDLBuf)
    {
        rio::MemUtil::free(mpDLBuf);
        mpDLBuf = nullptr;
    }
}

void ShaderProgramArchive::destroyResFile_()
{
    if (mResText.isValid())
    {
        mProgramEx.freeBuffer();
        mSourceText.freeBuffer();
        mSourceName.freeBuffer();
        mSource.freeBuffer();
    }
    mResText = NULL;
}

void ShaderProgramArchive::createWithOption(ResBinaryShaderArchive res_binary_archive, ResShaderArchive res_archive, u32 flag)
{
    mResBinary = res_binary_archive;

    if (flag & 2)
        mFlag.set(1);

    if (mResBinary.isValid())
    {
        mResBinary.setUp(true);

        mProgram.allocBuffer(mResBinary.getResBinaryShaderProgramNum());

        bool create_dl = !(flag & 1);

        u8* dl_buf = NULL;
        u32 cur_dl_offs = 0;
        const u32 dl_size = 0x220;

        if (create_dl)
        {
            u32 dl_buf_size = mResBinary.getResShaderBinaryNum() * dl_size;
            dl_buf = static_cast<u8*>(rio::MemUtil::alloc(dl_buf_size, DisplayList::cDisplayListAlignment));
#if RIO_IS_CAFE
            DCFlushRange(dl_buf, dl_buf_size);
#endif // RIO_IS_CAFE
        }

        mpDLBuf = dl_buf;

        const ResBinaryShaderProgramArray binary_prog_arr = mResBinary.getResBinaryShaderProgramArray();

        for (ResBinaryShaderProgramArray::constIterator it = binary_prog_arr.begin(), it_end = binary_prog_arr.end(); it != it_end; ++it)
        {
            const ResBinaryShaderProgram binary_prog(&(*it));

            ShaderProgram& program = mProgram[it.getIndex()];

            createShaderProgram(
                &program,
                binary_prog.getName(),
                binary_prog.getResShaderVariationArray(),
                binary_prog.getResShaderVariationDefaultArray(),
                binary_prog.getResShaderSymbolArray(cShaderSymbolType_Attribute),
                binary_prog.getResShaderSymbolArray(cShaderSymbolType_Uniform),
                binary_prog.getResShaderSymbolArray(cShaderSymbolType_UniformBlock),
                binary_prog.getResShaderSymbolArray(cShaderSymbolType_Sampler)
            );

            for (s32 i = 0; i < program.getVariationNum(); i++)
            {
                ShaderProgram* var_program = program.getVariation(i);

                for (s32 type = 0; type < cShaderType_Num; type++)
                {
                    Shader* shader = var_program->getShader(ShaderType(type));
                    if (binary_prog.ref().mKind & 1 << type)
                    {
                        u32 num_shader = 3;
                        if ((binary_prog.ref().mKind >> cShaderType_Geometry & 1) == 0)
                            num_shader = 2;

                        u32 variation_index = binary_prog.ref().mBaseIndex + type + num_shader * i;
                        if (variation_index != 0xFFFFFFFF)
                        {
                            const ResShaderBinary binary = mResBinary.getResShaderBinaryArray().get(variation_index);
#if RIO_IS_CAFE
                            DCFlushRangeNoSync(binary.getData(), binary.ref().mDataSize);
#endif // RIO_IS_CAFE
                            shader->setBinary(binary.getData());
                        }
                    }
                }

                if (create_dl)
                {
                    var_program->getDisplayList().setBuffer(dl_buf + cur_dl_offs, dl_size);
                    cur_dl_offs += dl_size;
                }
            }
        }
    }

    setResShaderArchive_(res_archive);

    for (Buffer<ShaderProgram>::iterator it = mProgram.begin(), it_end = mProgram.end(); it != it_end; ++it)
        it->reserveSetUpAllVariation();
}

bool ShaderProgramArchive::setUp()
{
    updateCompileInfo();
    return setUp_(mFlag.isOn(1));
}

s32 ShaderProgramArchive::searchShaderProgramIndex(const char* name) const
{
    for (Buffer<ShaderProgram>::constIterator it = mProgram.begin(), it_end = mProgram.end(); it != it_end; ++it)
        if (std::strcmp(it->getName(), name) == 0)
            return it.getIndex();

    return -1;
}

void ShaderProgramArchive::updateCompileInfo()
{
    for (Buffer<ShaderSource>::iterator it = mSource.begin(), it_end = mSource.end(); it != it_end; ++it)
        if (it->mFlag.isOn(1))
            it->expand();

    for (Buffer<ShaderProgramEx>::iterator it = mProgramEx.begin(), it_end = mProgramEx.end(); it != it_end; ++it)
        it->updateRawText();

    for (Buffer<ShaderSource>::iterator it = mSource.begin(), it_end = mSource.end(); it != it_end; ++it)
        it->mFlag.reset(1);
}

void ShaderProgramArchive::setResShaderArchive_(ResShaderArchive res_archive)
{
    destroyResFile_();

    if (!res_archive.isValid())
        return;

    mResText = res_archive;
    mResText.setUp();

    bool do_init_programs = false;

    if (!mResBinary.isValid())
    {
        mProgram.allocBuffer(mResText.getResShaderProgramNum());
        do_init_programs = true;
    }
    else
    {
        RIO_ASSERT(mResText.getResShaderProgramNum() == mResBinary.getResBinaryShaderProgramNum());
    }

    mSource.allocBuffer(mResText.getResShaderSourceNum());

    mSourceText.allocBuffer(mSource.size());
    mSourceName.allocBuffer(mSource.size());

    mProgramEx.allocBuffer(mProgram.size());

    bool source_is_used[1024]; // SafeArray<bool, 1024>

    const ResShaderSourceArray source_arr = mResText.getResShaderSourceArray();
    const ResShaderProgramArray prog_arr = mResText.getResShaderProgramArray();

    for (ResShaderSourceArray::constIterator it = source_arr.begin(), it_end = source_arr.end(); it != it_end; ++it)
    {
        const ResShaderSource source(&(*it));
        source_is_used[it.getIndex()] = false;

        for (ResShaderProgramArray::constIterator it_prog = prog_arr.begin(), it_prog_end = prog_arr.end(); it_prog != it_prog_end; ++it_prog)
        {
            const ResShaderProgram prog(&(*it_prog));
            bool found = false;

            for (s32 type = 0; type < cShaderType_Num; type++)
            {
                if (prog.ref().mSourceIndex[type] == it.getIndex())
                {
                    source_is_used[it.getIndex()] = true;
                    found = true;
                    break;
                }
            }

            if (found)
                break;
        }
    }

    for (ResShaderSourceArray::constIterator it = source_arr.begin(), it_end = source_arr.end(); it != it_end; ++it)
    {
        const ResShaderSource source(&(*it));
        mSource[it.getIndex()].initialize(this, it.getIndex(), source, source_is_used[it.getIndex()]);
    }

    for (ResShaderProgramArray::constIterator it = prog_arr.begin(), it_end = prog_arr.end(); it != it_end; ++it)
    {
        const ResShaderProgram prog(&(*it));

        if (do_init_programs)
        {
            ShaderProgram& program = mProgram[it.getIndex()];

            createShaderProgram(
                &program,
                prog.getName(),
                prog.getResShaderVariationArray(),
                prog.getResShaderVariationDefaultArray(),
                prog.getResShaderSymbolArray(cShaderSymbolType_Attribute),
                prog.getResShaderSymbolArray(cShaderSymbolType_Uniform),
                prog.getResShaderSymbolArray(cShaderSymbolType_UniformBlock),
                prog.getResShaderSymbolArray(cShaderSymbolType_Sampler)
            );
        }

        mProgramEx[it.getIndex()].initialize(this, it.getIndex(), prog);
    }

    updateCompileInfo();
}

bool ShaderProgramArchive::setUp_(bool unk)
{
    _28 += 1;

    for (Buffer<ShaderProgramEx>::iterator it = mProgramEx.begin(), it_end = mProgramEx.end(); it != it_end; ++it)
        it->updateAnalyze();

    for (Buffer<ShaderProgram>::iterator it = mProgram.begin(), it_end = mProgram.end(); it != it_end; ++it)
    {
        // TODO
        // it->mpSharedData->_10 = _20;

        if ((!unk || _28 <= 1))
        {
#if RIO_IS_WIN
            bool compile_enable = it->getCompileEnable();
            it->setCompileEnable(true);
            u32 ret = it->setUpAllVariation();
            it->setCompileEnable(compile_enable);
#else
            u32 ret = it->setUpAllVariation();
#endif // RIO_IS_WIN
            if (ret != 0)
                return false;
        }
    }

    // TODO
    // if (_24)
    //     _24->vf0C(this);

    return true;
}

ShaderProgramArchive::ShaderProgramEx::ShaderProgramEx()
    : mIndex(0)
    , mpArchive(NULL)
    , mCompileInfoEx()
    , _110()
    , mVariationIndex(0)
    , mFlag(0)
{
}

ShaderProgramArchive::ShaderProgramEx::~ShaderProgramEx()
{
    if (_110.isBufferReady())
        _110.freeBuffer();
}

void ShaderProgramArchive::ShaderProgramEx::initialize(ShaderProgramArchive* archive, s32 index, ResShaderProgram res)
{
    mIndex = index;
    mpArchive = archive;

    ShaderProgram& program = archive->mProgram[index];

    // TODO: SafeArray
    {
        typedef Buffer<ShaderCompileInfoEx>::iterator _Iterator;
        for (_Iterator it = _Iterator(mCompileInfoEx), it_end = _Iterator(mCompileInfoEx, cShaderType_Num); it != it_end; ++it)
        {
            s32 source_index = res.ref().mSourceIndex[it.getIndex()];
            ShaderType type = ShaderType(it.getIndex());

            if (source_index != -1)
            {
                it->mSource = &mpArchive->mSource[source_index];

                const ResShaderMacroArray macro_arr = res.getResShaderMacroArray(type);
                it->mCompileInfo.create(macro_arr.getNum(), program.getVariationMacroNum());

                it->mCompileInfo.setName(it->mSource->getName());
                it->mCompileInfo.setRawText(&it->mRawText);

                for (ResShaderMacroArray::constIterator macro_it = macro_arr.begin(), macro_it_end = macro_arr.end(); macro_it != macro_it_end; ++macro_it)
                {
                    const ResShaderMacro macro(&(*macro_it));
                    it->mCompileInfo.pushBackMacro(macro.getName(), macro.getValue());
                }

                program.getShader(type)->setCompileInfo(&it->mCompileInfo);
            }
            else
            {
                it->mSource = NULL;
            }
        }
    }

    if (program.getVariationMacroNum() > 0)
    {
        _110.allocBuffer(program.getVariationMacroNum());
        for (Buffer<u32>::iterator it = _110.begin(), it_end = _110.end(); it != it_end; ++it)
            *it = 0;
    }

    // detail::RootNode::setNodeMeta(this, "Icon = CIRCLE_ORENGE");
}

void ShaderProgramArchive::ShaderProgramEx::updateRawText()
{
    // TODO: SafeArray
    {
        typedef Buffer<ShaderCompileInfoEx>::iterator _Iterator;
        for (_Iterator it = _Iterator(mCompileInfoEx), it_end = _Iterator(mCompileInfoEx, cShaderType_Num); it != it_end; ++it)
        {
            ShaderSource* source = it->mSource;
            if (source && source->mFlag.isOn(1))
            {
                it->mCompileInfo.setSourceText(source->mRawText);
                mpArchive->getShaderProgram(mIndex).reserveSetUpAllVariation();
            }
        }
    }
}

void ShaderProgramArchive::ShaderProgramEx::updateAnalyze()
{
    if (mFlag.isOn(1))
        mpArchive->mProgram[mIndex].updateVariation(mVariationIndex);
}

ShaderProgramArchive::ShaderSource::ShaderSource()
    : mFlag(1)
    , mpArchive(NULL)
    , mRes()
    , mText(NULL)
    , mRawText(NULL)
    , mUsedInSource()
{
}

ShaderProgramArchive::ShaderSource::~ShaderSource()
{
    delete mRawText;
    delete mText;
    mUsedInSource.freeBuffer();
}

void ShaderProgramArchive::ShaderSource::initialize(ShaderProgramArchive* archive, s32 index, ResShaderSource res, bool is_used)
{
    mpArchive = archive;
    mRes = res;
    mIndex = index;

    mFlag.change(1 << 1, is_used);

    mText = new std::string(res.getText(), res.ref().mTextLen);
    mText->reserve(res.ref().mTextLen * 2);

    mpArchive->mSourceName[mIndex] = mRes.getName();
    mpArchive->mSourceText[mIndex] = mText->c_str();

    mUsedInSource.allocBuffer(mpArchive->mSource.size());
    for (Buffer<bool>::iterator it = mUsedInSource.begin(), it_end = mUsedInSource.end(); it != it_end; ++it)
        *it = false;

    // detail::RootNode::setNodeMeta(this, "Icon = NOTE");
}

void ShaderProgramArchive::ShaderSource::expand()
{
    if (mFlag.isOff(1 << 1))
        return;

    if (mRawText)
    {
        delete mRawText;
        mRawText = NULL;
    }

    RIO_ASSERT(mpArchive->mSource.size() < 1024);
    bool source_is_used[1024];

    mRawText = detail::ShaderTextUtil::createRawText(
        mText->c_str(),
        mpArchive->mSourceName.getBufferPtr(),
        mpArchive->mSourceText.getBufferPtr(),
        mpArchive->mSource.size(),
        source_is_used
    );

    for (Buffer<ShaderSource>::iterator it = mpArchive->mSource.begin(), it_end = mpArchive->mSource.end(); it != it_end; ++it)
        it->mUsedInSource[mIndex] = source_is_used[it.getIndex()];
}

}
