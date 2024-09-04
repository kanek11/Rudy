#include "RudyPCH.h"

#include "ToneMap.h"

namespace Rudy
{

ToneMap::ToneMap(
    uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<UTexture>>& m_ToneMapInputs, std::unordered_map<TexType, SharedPtr<UTexture>>& m_ToneMapOutputs) :
    m_width(m_width),
    m_height(m_height),
    m_ToneMapInputs(m_ToneMapInputs), m_ToneMapOutputs(m_ToneMapOutputs)
{
    Init();
}

void ToneMap::Init()
{
    m_ToneMapShader = Shader::CreateComputeShader("ToneMap Shader",
                                                  "Shaders/PostProcess/toneMapping_CS.glsl");

    // in case it's global computation,  use an extra texture;
    m_ToneMapScreenTexture = NewObject<UTexture2D>("",
                                                   TextureConfig { m_width, m_height, TexFormat::RGBA32F, 1, false, WrapMode::CLAMP_TO_BORDER, FilterMode::LINEAR });

    m_ToneMapOutputs[TexType::ScreenTexture] = m_ToneMapScreenTexture;
}

void ToneMap::Render()
{
    m_ToneMapShader->Bind();

    m_ToneMapShader->SetFloat("u_exposure", exposure);

    glBindImageTexture(0, m_ToneMapInputs[TexType::ScreenTexture]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    glBindImageTexture(1, m_ToneMapOutputs[TexType::ScreenTexture]->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(static_cast<uint32_t>(std::ceil(m_width / 8)), static_cast<uint32_t>(std::ceil(m_height / 8)), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_ToneMapShader->Unbind();
}

} // namespace Rudy
