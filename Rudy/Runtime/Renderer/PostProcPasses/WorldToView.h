#pragma once
#include "RudyPCH.h"

#include "Pass.h"
#include "Runtime/Actors/ACameraActor.h"

namespace Rudy
{

class WorldToView : public Pass
{
public:
    virtual ~WorldToView() = default;
    WorldToView(
        uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<UTexture>>& m_WorldToViewInputs, std::unordered_map<TexType, SharedPtr<UTexture>>& m_WorldToViewOutputs);

    void Render(const SharedPtr<ACameraActor>& camera);

    void Init();

public:
    uint32_t m_width, m_height;

    std::unordered_map<TexType, SharedPtr<UTexture>>& m_Inputs;
    std::unordered_map<TexType, SharedPtr<UTexture>>& m_Outputs;

    SharedPtr<Shader> m_WorldToViewShader;
};
} // namespace Rudy