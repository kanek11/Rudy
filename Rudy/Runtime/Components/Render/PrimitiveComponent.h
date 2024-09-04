#pragma once

#include "RudyPCH.h"

#include "Runtime/CoreFramework/Actor.h"
#include "Runtime/CoreFramework/ActorComponent.h"
#include "Runtime/CoreFramework/SceneComponent.h"

#include "Runtime/Renderer/Material.h"
#include "Runtime/Renderer/Buffer.h"
#include "Runtime/Renderer/Mesh.h"

namespace Rudy
{
//<<abstract>>
class UPrimitiveComponent : public USceneComponent
{
public:
    virtual ~UPrimitiveComponent() = default;
    UPrimitiveComponent();
};

//<<abstract>>
class UMeshComponent : public UPrimitiveComponent
{
public:
    virtual ~UMeshComponent() = default;
    UMeshComponent();

    virtual void Draw() = 0;

    void SetMaterialState(SharedPtr<Shader> _shader)
    {
        RD_CORE_ASSERT(m_material != nullptr, "No material attached to mesh component");

        m_material->SetRenderState(_shader);
    }

    void              SetShader(SharedPtr<Shader> shader) { m_shader = shader; }
    SharedPtr<Shader> GetShader() { return m_shader; }

    void                SetMaterial(SharedPtr<Material> mat) { m_material = mat; }
    SharedPtr<Material> GetMaterial() { return m_material; }

private:
    SharedPtr<Shader>   m_shader   = nullptr;
    SharedPtr<Material> m_material = nullptr;
};

//<<terminal>>
class UStaticMeshComponent : public UMeshComponent
{
public:
    ~UStaticMeshComponent() = default;
    UStaticMeshComponent();

    void TickComponent(float DeltaTime) override;

    virtual void Draw() override;

    void SetGeometryState(SharedPtr<Shader> _shader)
    {
        RD_CORE_ASSERT(m_mesh != nullptr, "No mesh attached to mesh component");

        m_mesh->renderData->m_vertexArray->Bind();

        if (auto owner = GetOwner().lock())
        {
            if (owner->RootComponent)
            {
                _shader->SetMat4("u_model", owner->RootComponent->GetWorldTransform());
            }
            else
            {
                RD_CORE_ERROR("Actor£º{0} has no root component", owner->Name);
            }
        }
    }

    void             SetMesh(SharedPtr<UMesh> m);
    SharedPtr<UMesh> GetMesh() { return m_mesh; }

private:
    bool m_bMeshDirty        = false; // change on mesh;
    bool m_bRenderDataLoaded = false; // load buffers

    SharedPtr<UMesh> m_mesh = nullptr;
};

// class USkinnedMeshComponent : public UMeshComponent
//{
// public:
//     ~USkinnedMeshComponent() = default;
//     USkinnedMeshComponent();
//
// private:
//     SharedPtr<Mesh>         m_mesh         = nullptr;
//     SharedPtr<VertexArray>  m_vertexArray  = nullptr;
//     SharedPtr<VertexBuffer> m_vertexBuffer = nullptr;
//     SharedPtr<IndexBuffer>  m_indexBuffer  = nullptr;
// };

} // namespace Rudy