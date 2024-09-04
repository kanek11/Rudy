#include "RudyPCH.h"

#include "SSR.h"

namespace Rudy
{

SSR::SSR(
    uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSRInputs, std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSROutputs) :
    m_width(m_width),
    m_height(m_height),
    m_SSRInputs(m_SSRInputs), m_SSROutputs(m_SSROutputs)
{
    Init();
}

void SSR::Init()
{
    ssrShader = Shader::CreateComputeShader("ssr Shader",
                                            "Shaders/PostProcess/SSR_CS.glsl");

    blurShader = Shader::CreateComputeShader("ssr blur Shader",
                                             "Shaders/PostProcess/SSR_aveFilter_CS.glsl");

    ssrScreenTexture = NewObject<UTexture2D>("", TextureConfig { m_width, m_height, TexFormat::RGBA32F, 1, false, WrapMode::CLAMP_TO_BORDER, FilterMode::LINEAR });

    ssrBlurTexture = NewObject<UTexture2D>("", TextureConfig { m_width, m_height, TexFormat::RGBA32F, 1, false, WrapMode::CLAMP_TO_BORDER, FilterMode::LINEAR });

    // m_SSROutputs[TexType::ScreenTexture] = ssrScreenTexture;
    m_SSROutputs[TexType::ScreenTexture] = ssrBlurTexture;
}

void SSR::Render(const SharedPtr<ACameraActor>& camera)
{
    ssrShader->Bind();

    ssrShader->SetMat4("u_projection", camera->CameraComponent->GetProjectionMatrix());
    ssrShader->SetFloat("u_depth_bias", depth_bias);
    ssrShader->SetFloat("u_step_size", step_size);
    ssrShader->SetInt("u_max_steps", max_steps);

    m_SSRInputs[TexType::gViewPosition]->BindToSlot(0);
    m_SSRInputs[TexType::gViewNormal]->BindToSlot(1);

    m_SSRInputs[TexType::ScreenTexture]->BindToSlot(2);
    m_SSRInputs[TexType::gViewDepth]->BindToSlot(3);

    glBindImageTexture(0, ssrScreenTexture->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(m_width / 8, m_height / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    ssrShader->Unbind();

    if (enableBlur)
    {
        m_SSROutputs[TexType::ScreenTexture] = ssrBlurTexture;
        blurShader->Bind();

        glBindImageTexture(0, ssrScreenTexture->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        glBindImageTexture(1, m_SSRInputs[TexType::gRoughness]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);

        glBindImageTexture(2, ssrBlurTexture->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        glDispatchCompute(m_width / 8, m_height / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        blurShader->Unbind();
    }
    else
    {
        m_SSROutputs[TexType::ScreenTexture] = ssrScreenTexture;
    }
}

} // namespace Rudy