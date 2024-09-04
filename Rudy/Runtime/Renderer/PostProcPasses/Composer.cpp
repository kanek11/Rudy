#include "RudyPCH.h"

#include "Composer.h"

namespace Rudy
{

Composer::Composer(
    uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<UTexture>>& m_ComposerInputs, std::unordered_map<TexType, SharedPtr<UTexture>>& m_ComposerOutputs) :
    m_width(m_width),
    m_height(m_height),
    m_ComposerInputs(m_ComposerInputs), m_ComposerOutputs(m_ComposerOutputs)
{
    Init();
}

void Composer::Init()
{
    ComposerShader = Shader::CreateComputeShader(
        "composer shader",
        "Shaders/PostProcess/composer_CS.glsl");

    m_ComposerOutputs[TexType::ScreenTexture] = m_ComposerInputs[TexType::litPassTexture];
}

void Composer::Render()
{
    ComposerShader->Bind();

    ComposerShader->SetFloat("u_outline_scale", outline_scale);
    ComposerShader->SetFloat("u_SSR_scale", SSR_scale);

    glBindImageTexture(0, m_ComposerInputs[TexType::litPassTexture]->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(1, m_ComposerInputs[TexType::SSROutputTexture]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(2, m_ComposerInputs[TexType::OutlineOutputTexture]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);

    glDispatchCompute(m_width / 8, m_height / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    ComposerShader->Unbind();
}

}; // namespace Rudy