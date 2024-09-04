#include "RudyPCH.h"

#include "Material.h"

namespace Rudy
{

SharedPtr<Material> Material::Create(const std::string& name)
{
    return CreateShared<Material>(name);
}

void Material::SetRenderState(SharedPtr<Shader> _shader)
{
    for (auto& texture : m_texture_map)
    {
        _shader->SetInt(TexTypeNames[texture.first], (int)texture.first);
    }

    for (auto& texture : m_texture_map)
    {
        texture.second->BindToSlot((int)texture.first);
    }

    for (auto& value : m_float_map)
    {
        _shader->SetFloat(value.first, value.second);
    }

    for (auto& value : m_vec3_map)
    {
        _shader->SetVec3(value.first, value.second);
    }

    for (auto& value : m_bool_map)
    {
        _shader->SetBool(value.first, value.second);
    }
}

void Material::UnsetRenderState(SharedPtr<Shader> _shader)
{
    for (auto& texture : m_texture_map)
    {
        texture.second->Unbind((int)texture.first);
    }
}

} // namespace Rudy