#include "RudyPCH.h"

#include "RendererComponent.h"

#include "Renderer.h"

namespace Rudy
{

void StaticMeshRenderer::SetMesh(SharedPtr<Mesh> m)
{
    RD_CORE_INFO("MeshObject: SetMesh:");
    m_mesh = m;

    m_mesh->SetupVertices();
    SetupBuffers();
}

void StaticMeshRenderer::SetupBuffers()
{
    // RD_PROFILE_FUNCTION();

    if (!hasMesh())
    {
        RD_CORE_ERROR("MeshObject::SetupBuffers: no mesh attached");
        return;
    }

    // data copy
    m_vertexBuffer->SetData(m_mesh->vertices.data(), m_mesh->vertices.size() * sizeof(Vertex), BufferUsage::STATIC_DRAW);
    m_indexBuffer->SetData(m_mesh->indices.data(), m_mesh->indices.size() * sizeof(uint32_t), BufferUsage::STATIC_DRAW);

    // attach
    m_vertexArray->AttachBuffer(m_vertexBuffer->GetBufferID(), 0, sizeof(Vertex));
    m_vertexArray->AttachIndexBuffer(m_indexBuffer->GetBufferID());

    m_vertexArray->AddAttribute(0, 0, 3, BufferDataType::FLOAT32, offsetof(Vertex, Position));
    m_vertexArray->AddAttribute(1, 0, 2, BufferDataType::FLOAT32, offsetof(Vertex, UV));
    m_vertexArray->AddAttribute(2, 0, 3, BufferDataType::FLOAT32, offsetof(Vertex, Normal));
    m_vertexArray->AddAttribute(3, 0, 3, BufferDataType::FLOAT32, offsetof(Vertex, Tangent));
    m_vertexArray->AddAttribute(4, 0, 4, BufferDataType::INT32, offsetof(Vertex, BoneIndices));
    m_vertexArray->AddAttribute(5, 0, 4, BufferDataType::FLOAT32, offsetof(Vertex, BoneWeights));

    RD_CORE_INFO("MeshObject::SetupBuffers: mesh buffers setup finished");
}

void StaticMeshRenderer::Draw(SharedPtr<Camera> cam, uint32_t count, SharedPtr<Material> mat)
{
    if (!hasMesh())
    {
        RD_CORE_ERROR("MeshObject::Draw: no mesh attached");
        return;
    }

    if (!this->hasMaterial())
    {
        RD_CORE_ERROR("MeshObject::Draw: no material attached");
        return;
    }

    SharedPtr<Material> _mat = nullptr;
    // use paramter material if not null
    // else use the material attached to the object, this implements cases like shadow map
    if (mat != nullptr)
    {
        _mat = mat;
    }
    else
    {
        _mat = m_material;
    }

    m_vertexArray->Bind();
    _mat->Bind();

    for (auto& SSBO : m_StorageBuffer_map)
    {
        SSBO.second->BindBase(SSBO.first);
    }

    if (cam != nullptr)
    {
        if (this->m_transform == nullptr)
        {
            RD_CORE_ERROR("MeshObject::Draw: no transform attached");
            return;
        }
        glm::mat4 model = this->m_transform->GetWorldTransform();

        if (_mat->GetShader() == nullptr)
        {
            RD_CORE_ERROR("MeshObject::Draw: no shader attached");
            return;
        }

        _mat->GetShader()->SetMat4("u_model", model);
        _mat->GetShader()->SetMat4("u_projection", cam->GetProjectionMatrix());
        _mat->GetShader()->SetMat4("u_view", cam->GetViewMatrix());
    }

    if (count == 1)
    {
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
    else if (count > 1)
    {
        switch (m_mesh->drawCommand)
        {
            case MeshDrawCommand::INDEXED:
                ViewportLayer::GetRendererAPI()->DrawIndexedInstanced(m_mesh->topology, m_mesh->GetIndexCount(), count);
                break;
            case MeshDrawCommand::ARRAYS:
                ViewportLayer::GetRendererAPI()->DrawArraysInstanced(m_mesh->topology, m_mesh->GetVertexCount(), count);
                break;
        }
    }

    _mat->Unbind();
    m_vertexArray->Unbind();
}

void ParticleSpriteRenderer::Draw(SharedPtr<Camera> cam, uint32_t count, SharedPtr<Material> mat)
{
    // disable depth test
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    if (!hasMaterial())
    {
        RD_CORE_ERROR("SpriteObject::Draw: no material attached");
        return;
    }

    SharedPtr<Material> _mat = nullptr;
    // use paramter material if not null
    // else use the material attached to the object, this implements cases like shadow map
    if (mat != nullptr)
    {
        _mat = mat;
    }
    else
    {
        _mat = m_material;
    }

    _mat->Bind();

    if (cam != nullptr)
    {
        if (this->m_transform == nullptr)
        {
            RD_CORE_ERROR("MeshObject::Draw: no transform attached");
            return;
        }
        glm::mat4 model = this->m_transform->GetWorldTransform();

        _mat->GetShader()->SetMat4("u_model", model);
        _mat->GetShader()->SetMat4("u_projection", cam->GetProjectionMatrix());
        _mat->GetShader()->SetMat4("u_view", cam->GetViewMatrix());
    }

    if (count == 1)
    {
        ViewportLayer::GetRendererAPI()->DrawArrays(MeshTopology::POINTS, 1);
    }
    else if (count > 1)
    {
        ViewportLayer::GetRendererAPI()->DrawArraysInstanced(MeshTopology::POINTS, 1, count);
    }

    _mat->Unbind();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

} // namespace Rudy