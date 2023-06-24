#include <common/aglShaderProgram.h>
#include <g3d/aglModelEx.h>

namespace agl { namespace g3d {

MaterialEx::MaterialEx()
    : mpModelEx(nullptr)
    , mpMaterialObj(nullptr)
    , mpProgram(nullptr)
    , mMatBlock()
    , _20(true)
{
}

MaterialEx::~MaterialEx()
{
    fixUpUBO();
}

void MaterialEx::init(agl::g3d::ModelEx* p_model, u32 index)
{
    mpModelEx = p_model;
    mpMaterialObj = p_model->GetMaterial(index);
    mMatBlock = static_cast<nw::g3d::fnd::GfxBuffer_t&>(mpMaterialObj->GetMatBlock());
}

void MaterialEx::bindShaderResAssign(const ShaderProgram* p_program, const char* skin_macro, const char** skin_value_array)
{
    std::unordered_map<std::string, std::string> skin_macro_map;
    const std::string& s_skin_macro = skin_macro ? std::string(skin_macro) : std::string();

    mpProgram = p_program;

    const nw::g3d::res::ResMaterial* const p_res_material = mpMaterialObj->GetResource();

    const nw::g3d::res::ResShaderAssign* const p_res_shader_assign = p_res_material->GetShaderAssign();
    if (p_res_shader_assign == nullptr || p_program == nullptr)
    {
        for (s32 idx_shape = 0; idx_shape < mpModelEx->GetShapeCount(); idx_shape++)
        {
            const nw::g3d::ShapeObj* const p_shape = mpModelEx->GetShape(idx_shape);
            if (&mpModelEx->getMaterialEx(p_shape->GetMaterialIndex()) == this)
            {
                mpModelEx->getShaderAssign(idx_shape).bindShaderResAssign(
                    p_res_material,
                    p_shape->GetResource(),
                    nullptr
                );
            }
        }
        return;
    }

    s32 macro_num = p_res_shader_assign->GetShaderOptionCount();

    std::unordered_map<std::string, std::string> macro_map;
    macro_map.reserve(macro_num);

    for (s32 idx_macro = 0; idx_macro < macro_num; idx_macro++)
    {
        const char* const id = p_res_shader_assign->GetShaderOptionName(idx_macro);
        const char* const value = p_res_shader_assign->GetShaderOption(idx_macro);

        const std::string* const p_macro_name = p_program->searchVariationMacroName(id);
        if (p_macro_name)
        {
            [[maybe_unused]] const auto& itr = macro_map.try_emplace(
                *p_macro_name,
                value
            );

            RIO_ASSERT(itr.second);
        }
    }

    const ShaderProgram* p_base_variation = p_program->searchVariationShaderProgram(macro_map);

    for (s32 idx_shape = 0; idx_shape < mpModelEx->GetShapeCount(); idx_shape++)
    {
        const nw::g3d::ShapeObj* p_shape = mpModelEx->GetShape(idx_shape);
        if (&mpModelEx->getMaterialEx(p_shape->GetMaterialIndex()) == this)
        {
            const ShaderProgram* p_variation = p_base_variation;
            if (p_base_variation && skin_value_array)
            {
                skin_macro_map[s_skin_macro] = skin_value_array[p_shape->GetVtxSkinCount()];
                p_variation = p_base_variation->searchVariationShaderProgram(skin_macro_map);
            }

            mpModelEx->getShaderAssign(idx_shape).bindShaderResAssign(
                p_res_material,
                p_shape->GetResource(),
                p_variation
            );
        }
    }
}

void MaterialEx::bindShader(const ShaderProgram* p_program)
{
    mpProgram = p_program;

    for (s32 idx_shape = 0; idx_shape < mpModelEx->GetShapeCount(); idx_shape++)
    {
        if (&mpModelEx->getMaterialEx(mpModelEx->GetShape(idx_shape)->GetMaterialIndex()) == this)
        {
            mpModelEx->getShaderAssign(idx_shape).bindShader(
                mpMaterialObj->GetResource(),
                mpModelEx->GetShape(idx_shape)->GetResource(),
                p_program
            );
        }
    }
}

void MaterialEx::replaceUBO(const nw::g3d::fnd::GfxBuffer_t& buffer)
{
    static_cast<nw::g3d::fnd::GfxBuffer_t&>(mpMaterialObj->GetMatBlock()) = buffer;
}

void MaterialEx::fixUpUBO()
{
    replaceUBO(mMatBlock);
}

ModelEx::ModelEx()
    : nw::g3d::ModelObj()
    , mpShaderAssign(nullptr)
    , mpMaterialEx(nullptr)
{
}

ModelEx::~ModelEx()
{
    destroyEx();
}

void ModelEx::createEx()
{
    const nw::g3d::res::ResModel* p_res_mdl = GetResource();
    s32 shape_num = p_res_mdl->GetShapeCount();
    s32 material_num = p_res_mdl->GetMaterialCount();

    mpShaderAssign = new ModelShaderAssign[shape_num];
    mpMaterialEx = new MaterialEx[material_num];

    for (s32 idx_material = 0; idx_material < material_num; idx_material++)
        mpMaterialEx[idx_material].init(this, idx_material);

    for (s32 idx_shape = 0; idx_shape < shape_num; idx_shape++)
        mpShaderAssign[idx_shape].create();
}

void ModelEx::destroyEx()
{
    if (mpShaderAssign)
    {
        delete[] mpShaderAssign;
        mpShaderAssign = nullptr;
    }
    if (mpMaterialEx)
    {
        delete[] mpMaterialEx;
        mpMaterialEx = nullptr;
    }
}

} }
