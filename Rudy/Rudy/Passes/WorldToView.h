#pragma once
#include "RudyPCH.h"

#include "Pass.h"

namespace Rudy
{

class WorldToView : public Pass
{
public:
    virtual ~WorldToView() = default;
    WorldToView(
        uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, Ref<Texture>>& m_WorldToViewInputs, std::unordered_map<TexType, Ref<Texture>>& m_WorldToViewOutputs);

    virtual void Render(Ref<Camera>) override;

    void Init();

public:
    uint32_t                                   m_width, m_height;
    std::unordered_map<TexType, Ref<Texture>>& m_Inputs;
    std::unordered_map<TexType, Ref<Texture>>& m_Outputs;

    Ref<Shader> m_WorldToViewShader;
};
} // namespace Rudy