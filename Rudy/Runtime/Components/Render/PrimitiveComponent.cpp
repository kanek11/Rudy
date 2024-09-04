#pragma once
#include "RudyPCH.h"

#include "Apps/Layer.h"
#include "Runtime/Renderer/RendererAPI.h"
#include "PrimitiveComponent.h"

namespace Rudy
{
UPrimitiveComponent::UPrimitiveComponent() :
    USceneComponent()
{
}

UMeshComponent::UMeshComponent() :
    UPrimitiveComponent()
{
}

UStaticMeshComponent::UStaticMeshComponent() :
    UMeshComponent()
{
}

void UStaticMeshComponent::SetMesh(SharedPtr<UMesh> m)
{
    m_mesh       = m;
    m_bMeshDirty = true;
}

void UStaticMeshComponent::TickComponent(float DeltaTime)
{
    UMeshComponent::TickComponent(DeltaTime);
    if (m_bMeshDirty)
    {
        m_mesh->SetupRenderData();
        m_bMeshDirty        = false;
        m_bRenderDataLoaded = true;
    }
}

void UStaticMeshComponent::Draw()
{
    if (!m_bRenderDataLoaded)
    {
        m_mesh->SetupRenderData();
        m_bRenderDataLoaded = true;
        RD_CORE_ERROR("{0}: Mesh buffers loaded just before drawn, to be handled ", GetOwner().lock()->Name);
    }

    switch (m_mesh->drawCommand)
    {
        case MeshDrawCommand::INDEXED:
            ViewportLayer::GetRendererAPI()->DrawIndexed(m_mesh->topology, m_mesh->GetIndexCount());
            break;
        case MeshDrawCommand::ARRAYS:
            ViewportLayer::GetRendererAPI()->DrawArrays(m_mesh->topology, m_mesh->GetVertexCount());
            break;
    }
}

} // namespace Rudy