#include "RudyPCH.h"

#include "RendererComponent.h"

#include "Rudy/Renderer/Renderer.h"


namespace Rudy
{
	 

	void MeshRendererComponent::SetMesh(Ref<Mesh> m)
	{
		RD_CORE_INFO("MeshObject: SetMesh:");
		m_mesh = m;
         
		m_mesh->SetupVertices();
		SetupBuffers();

	}

	
    void MeshRendererComponent::SetupBuffers()
    {
        //RD_PROFILE_FUNCTION(); 

        if (!hasMesh())
        {
            RD_CORE_ERROR("MeshObject::SetupBuffers: no mesh attached");
            return;
        }


        //data copy
        m_vertexBuffer->SetData(m_mesh->vertices.data(),m_mesh->vertices.size() * sizeof(Vertex),  BufferUsage::STATIC_DRAW);
        m_indexBuffer ->SetData(m_mesh->indices.data(), m_mesh->indices.size() * sizeof(uint32_t), BufferUsage::STATIC_DRAW);

        //attach
        m_vertexArray->AttachBuffer(m_vertexBuffer->GetBufferID(), 0, sizeof(Vertex));
        m_vertexArray->AttachIndexBuffer (m_indexBuffer->GetBufferID());


        m_vertexArray->AddAttribute(0, 0, 3, BufferDataType::FLOAT32, offsetof(Vertex, Position));
        m_vertexArray->AddAttribute(1, 0, 2, BufferDataType::FLOAT32, offsetof(Vertex, UV));
        m_vertexArray->AddAttribute(2, 0, 3, BufferDataType::FLOAT32, offsetof(Vertex, Normal));
        m_vertexArray->AddAttribute(3, 0, 3, BufferDataType::FLOAT32, offsetof(Vertex, Tangent));
        m_vertexArray->AddAttribute(4, 0, 4, BufferDataType::INT32, offsetof(Vertex, BoneIndices));
        m_vertexArray->AddAttribute(5, 0, 4, BufferDataType::FLOAT32, offsetof(Vertex, BoneWeights));
          

        RD_CORE_INFO("MeshObject::SetupBuffers: mesh buffers setup finished");

    }



     
    void MeshRendererComponent::Draw(Ref<Camera> cam)
    {
        if (!hasMesh())
        {
            RD_CORE_ERROR("MeshObject::Draw: no mesh attached");
            return;
        }
        if (!hasMaterial())
        {
            RD_CORE_ERROR("MeshObject::Draw: no material attached");
            return;
        }

        m_vertexArray->Bind();
        m_material->Bind();

        if (cam != nullptr)
        {  
            if (this->m_transform == nullptr)
            {
                RD_CORE_ERROR("MeshObject::Draw: no transform attached");
                return;
            }
            glm::mat4 model = this->m_transform->GetWorldTransform(); 

            m_material->GetShader()->SetMat4("u_model", model);
            m_material->GetShader()->SetMat4("u_projection", cam->GetProjectionMatrix());
            m_material->GetShader()->SetMat4("u_view", cam->GetViewMatrix());

        }

        switch (m_mesh->drawCommand)
        {
        case MeshDrawCommand::INDEXED:
            Renderer::GetRendererAPI()->DrawIndexed(m_mesh->topology, m_mesh->GetIndexCount());
            break;
        case MeshDrawCommand::ARRAYS:
            Renderer::GetRendererAPI()->DrawArrays (m_mesh->topology, m_mesh->GetVertexCount());
            break;
        } 

        m_material->Unbind();
        m_vertexArray->Unbind();

    }



    void MeshRendererComponent::DrawInstanced(Ref<Camera> cam, uint32_t instanceCount)
    {
        if (!hasMesh())
        {
            RD_CORE_ERROR("MeshObject::Draw: no mesh attached");
            return;
        }
        if (!hasMaterial())
        {
            RD_CORE_ERROR("MeshObject::Draw: no material attached");
            return;
        }

        m_vertexArray->Bind();
        m_material->Bind();

        if (cam != nullptr)
        {
            if (this->m_transform == nullptr)
            {
                RD_CORE_ERROR("MeshObject::Draw: no transform attached");
                return;
            }
            glm::mat4 model = this->m_transform->GetWorldTransform();

            m_material->GetShader()->SetMat4("u_model", model);
            m_material->GetShader()->SetMat4("u_projection", cam->GetProjectionMatrix());
            m_material->GetShader()->SetMat4("u_view", cam->GetViewMatrix());

        }

        switch (m_mesh->drawCommand)
        {
        case MeshDrawCommand::INDEXED:
            Renderer::GetRendererAPI()->DrawIndexedInstanced(m_mesh->topology, m_mesh->GetIndexCount(), instanceCount);
            break;
        case MeshDrawCommand::ARRAYS:
            Renderer::GetRendererAPI()->DrawArraysInstanced(m_mesh->topology, m_mesh->GetVertexCount(), instanceCount);
            break;
        }


        m_material->Unbind();
        m_vertexArray->Unbind();

    }



    
    void ParticleSpriteRendererComponent::Draw(Ref<Camera> cam)
    {

		if (!hasMaterial())
		{
			RD_CORE_ERROR("SpriteObject::Draw: no material attached");
            return;
		}
         
		m_material->Bind(); 

        if (cam != nullptr)
        {
            if (this->m_transform == nullptr)
            {
                RD_CORE_ERROR("MeshObject::Draw: no transform attached");
                return;
            }
            glm::mat4 model = this->m_transform->GetWorldTransform();

            m_material->GetShader()->SetMat4("u_model", model);
            m_material->GetShader()->SetMat4("u_projection", cam->GetProjectionMatrix());
            m_material->GetShader()->SetMat4("u_view", cam->GetViewMatrix());

        }

		Renderer::GetRendererAPI()->DrawArrays(MeshTopology::POINTS, 1);

		m_material->Unbind(); 

	}


    void ParticleSpriteRendererComponent::DrawInstanced(Ref<Camera> cam, uint32_t instanceCount)
    {

        if (!hasMaterial())
        {
            RD_CORE_ERROR("SpriteObject::Draw: no material attached");
            return;
        }

        m_material->Bind();


        if (cam != nullptr)
        {
            if (this->m_transform == nullptr)
            {
                RD_CORE_ERROR("MeshObject::Draw: no transform attached");
                return;
            }
            glm::mat4 model = this->m_transform->GetWorldTransform();

            m_material->GetShader()->SetMat4("u_model", model);
            m_material->GetShader()->SetMat4("u_projection", cam->GetProjectionMatrix());
            m_material->GetShader()->SetMat4("u_view", cam->GetViewMatrix());

        }


        Renderer::GetRendererAPI()->DrawArraysInstanced(MeshTopology::POINTS, 1, instanceCount);

        m_material->Unbind(); 

    }




}