#pragma once
#include "RudyPCH.h"

#include "Pass.h"
#include "Runtime/Actors/ACameraActor.h"

namespace Rudy
{

class SSAO : public Pass
{
public:
    virtual ~SSAO() = default;
    SSAO(
        uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSAOInputs, std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSAOOutputs);

    void Render(const SharedPtr<ACameraActor>& camera);

    void Init();

public:
    // common
    uint32_t m_width, m_height;

    std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSAOInputs;
    std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSAOOutputs;

    // specific

    SharedPtr<FrameBuffer> SSAOFBO;
    SharedPtr<Shader>      SSAOShader;
    SharedPtr<Material>    SSAOMaterial;

    SharedPtr<ScreenQuad> SSAOQuad;

    SharedPtr<UTexture> SSAOOnlyTex;
    SharedPtr<UTexture> SSAOBlurTexture;

    float radius     = 0.5f;
    float bias       = 0.01f;
    float range_bias = 0.4f;

    int kernelSize = 64;

    // blur
    SharedPtr<Shader> BlurShader;

    bool enableBlur = false;
    int  ave_radius = 4;

    // float sigma = 7.0f;
    // int radius = 5;
};
} // namespace Rudy