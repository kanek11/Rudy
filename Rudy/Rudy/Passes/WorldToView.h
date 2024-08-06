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
        uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<Texture>>& m_WorldToViewInputs, std::unordered_map<TexType, SharedPtr<Texture>>& m_WorldToViewOutputs);

    virtual void Render(SharedPtr<Camera>) override;

    void Init();

public:
    uint32_t                                   m_width, m_height;
    std::unordered_map<TexType, SharedPtr<Texture>>& m_Inputs;
    std::unordered_map<TexType, SharedPtr<Texture>>& m_Outputs;

    SharedPtr<Shader> m_WorldToViewShader;
};
} // namespace Rudy