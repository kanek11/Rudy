#pragma once
#include "RudyPCH.h"

#include "Pass.h"

namespace Rudy
{

class SSAO : public Pass
{
public:
    virtual ~SSAO() = default;
    SSAO(
        uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, Ref<Texture>>& m_SSAOInputs, std::unordered_map<TexType, Ref<Texture>>& m_SSAOOutputs);

    virtual void Render(Ref<Camera>) override;

    void Init();

public:
    // common
    uint32_t                                   m_width, m_height;
    std::unordered_map<TexType, Ref<Texture>>& m_SSAOInputs;
    std::unordered_map<TexType, Ref<Texture>>& m_SSAOOutputs;

    // specific

    Ref<FrameBuffer> SSAOFBO;
    Ref<Shader>      SSAOShader;
    Ref<Material>    SSAOMaterial;

    Ref<ScreenQuad> SSAOQuad;

    Ref<Texture2D> SSAOOnlyTex;
    Ref<Texture2D> SSAOBlurTexture;

    float radius     = 0.5f;
    float bias       = 0.01;
    float range_bias = 0.4;

    int kernelSize = 64;

    // blur
    Ref<Shader> BlurShader;

    bool enableBlur = false;
    int  ave_radius = 4;

    // float sigma = 7.0;
    // int radius = 5;
};
} // namespace Rudy