#include "RudyPCH.h"

#include "Outline.h"

namespace Rudy
{

Outline::Outline(
    uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<Texture>>& m_OutlineInputs, std::unordered_map<TexType, SharedPtr<Texture>>& m_OutlineOutputs) :
    m_width(m_width),
    m_height(m_height),
    m_OutlineInputs(m_OutlineInputs), m_OutlineOutputs(m_OutlineOutputs)
{
    Init();
}

void Outline::Init()
{
    outlineShader = Shader::CreateComputeShader("outlineShader",
                                                //"Shaders/PostProcess/Outline_CS.glsl");
                                                "Shaders/PostProcess/Outline_FDM_CS.glsl");

    outputTexture = Texture2D::CreateEmpty(
        TextureSpec { m_width, m_height, TextureInternalFormat::R32F, false, WrapMode::ClampToBorder, FilterMode::Nearest });

    m_OutlineOutputs[TexType::ScreenTexture] = outputTexture;

    // m_OutlineOutputs[TexType::ScreenTexture] = m_OutlineInputs[TexType::gViewNormal];
}

void Outline::Render(SharedPtr<Camera>)
{
    outlineShader->Bind();

    outlineShader->SetFloat("u_outline_width", outline_width);
    outlineShader->SetFloat("u_depth_thres", depth_thres);
    outlineShader->SetFloat("u_depth_thres_scale", depth_thres_scale);
    outlineShader->SetFloat("u_depth_NdotV_threshold", depth_NdotV_threshold);
    outlineShader->SetFloat("u_normal_threshold", normal_threshold);
    outlineShader->SetFloat("u_outline_width", outline_width);
    outlineShader->SetBool("u_enable_normal", enable_normal);
    outlineShader->SetBool("u_enable_depth", enable_depth);

    glBindImageTexture(0, m_OutlineInputs[TexType::gViewPosition]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, m_OutlineInputs[TexType::gViewNormal]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    glBindImageTexture(2, m_OutlineOutputs[TexType::ScreenTexture]->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

    glDispatchCompute(m_width / 8, m_height / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    outlineShader->Unbind();
}

} // namespace Rudy
