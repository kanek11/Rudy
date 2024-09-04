#pragma once
#include "RudyPCH.h"

#include "Pass.h"
#include "Runtime/Actors/ACameraActor.h"
namespace Rudy
{

class SSR : public Pass
{
public:
    virtual ~SSR() = default;
    SSR(
        uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSRInputs, std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSROutputs);

    void Render(const SharedPtr<ACameraActor>& camera);

    void Init();

public:
    uint32_t                                          m_width, m_height;
    std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSRInputs;
    std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSROutputs;

    SharedPtr<Shader> ssrShader;
    SharedPtr<Shader> blurShader;

    SharedPtr<UTexture> ssrScreenTexture;
    SharedPtr<UTexture> ssrBlurTexture;

    float depth_bias = 0.02f;
    float step_size  = 0.02f;
    int   max_steps  = 250;

    bool enableBlur = true;
};

} // namespace Rudy