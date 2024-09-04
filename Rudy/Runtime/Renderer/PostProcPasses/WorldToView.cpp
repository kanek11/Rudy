#include "RudyPCH.h"

#include "WorldToView.h"

namespace Rudy
{

WorldToView::WorldToView(
    uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<UTexture>>& m_WorldToViewInputs, std::unordered_map<TexType, SharedPtr<UTexture>>& m_WorldToViewOutputs) :
    m_width(m_width),
    m_height(m_height),
    m_Inputs(m_WorldToViewInputs), m_Outputs(m_WorldToViewOutputs)
{
    Init();
}

void WorldToView::Init()
{
    m_WorldToViewShader = Shader::CreateComputeShader("WorldToView Shader",
                                                      "Shaders/PostProcess/WorldToView_CS.glsl");
}

void WorldToView::Render(const SharedPtr<ACameraActor>& camera)
{
    m_WorldToViewShader->Bind();

    m_WorldToViewShader->SetMat4("u_view", camera->CameraComponent->GetViewMatrix());

    glBindImageTexture(0, m_Inputs[TexType::gWorldPosition]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, m_Inputs[TexType::gWorldNormal]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    // glBindImageTexture(2, m_Inputs[TexType::gViewDepth]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);

    m_Inputs[TexType::gViewDepth]->BindToSlot(0);
    glBindImageTexture(3, m_Outputs[TexType::gViewPosition]->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(4, m_Outputs[TexType::gViewNormal]->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(static_cast<uint32_t>(std::ceil(m_width / 8)), static_cast<uint32_t>(std::ceil(m_height / 8)), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_WorldToViewShader->Unbind();
}

} // namespace Rudy