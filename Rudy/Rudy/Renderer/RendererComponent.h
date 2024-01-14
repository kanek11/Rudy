#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Transform.h"
#include "Rudy/Renderer/Mesh.h" 
#include "Rudy/Renderer/Material.h" 
#include "Rudy/Renderer/Camera.h"  
#include "Rudy/Renderer/Buffer.h" 


namespace Rudy
{

	//indrect is discarded, it overcomplicates things 
	struct DrawArraysIndirectCommand {
		uint32_t  vertex_count = 0;
		uint32_t  instance_count = 1;
		uint32_t  first = 0;
		uint32_t  base_instance = 0;
	};


	struct DrawIndexedIndirectCommand
    {
	    uint32_t  index_count = 0;
		uint32_t  instance_count = 1;
		uint32_t  first_index = 0;
		uint32_t  base_vertex = 0;
		uint32_t  base_instance = 0;
	}; 


	//for now, it simply impose a draw function
	//every draw call must use a shader program/ material
	//vertex array is somehow not limited to mesh, but also useful for instanced rendering etc

	//<<abstract base>>
	class RendererComponent
	{
	public:
		virtual ~RendererComponent() = default; 


		virtual void Draw(Ref<Camera> cam) = 0; 
		virtual void DrawInstanced(Ref<Camera> cam, uint32_t count) = 0; 


		//set/get
		void SetMaterial(Ref<Material> mat) { m_material = mat; }
		Ref<Material> GetMaterial() { return m_material; }

		void SetShader(Ref<Shader> shader) { m_material->SetShader(shader); }
		 

		void SetVertexArray(Ref<VertexArray> va) { m_vertexArray = va; }
        Ref<VertexArray> GetVertexArray() { return m_vertexArray; } 
     
		//bool
		bool hasMaterial() { return m_material != nullptr; }
        bool hasVertexArray() { return m_vertexArray != nullptr; } 

 

	protected:
		Ref<Material> m_material = nullptr;

		//optional
		Ref<VertexArray> m_vertexArray = nullptr; 
		

	public:
		//hack: 
		void SetTransform(Ref<Transform> t) { m_transform = t; }
		Ref<Transform> m_transform = nullptr;

    };


	//<<terminal>>
	class MeshRendererComponent : public RendererComponent
	{
	public: 
		~MeshRendererComponent() = default;
		MeshRendererComponent()
		{
			RD_CORE_INFO("MeshRendererComponent created:");
			m_vertexArray = VertexArray::Create();  
			m_vertexBuffer = VertexBuffer::Create();
			m_indexBuffer = IndexBuffer::Create();
		}

		static Ref<MeshRendererComponent> Create()
		{
			return CreateRef<MeshRendererComponent>();
		}

	public:
		virtual void Draw(Ref<Camera> cam) override;
		virtual void DrawInstanced(Ref<Camera> cam, uint32_t count) override;

		 
		void SetMesh(Ref<Mesh> m);
		Ref<Mesh> GetMesh() { return m_mesh; }   
		bool hasMesh() { return m_mesh != nullptr; }



		//system utilities
	private:

		void SetupBuffers(); 

	public:
		Ref<Mesh> m_mesh = nullptr;

		Ref<VertexBuffer> m_vertexBuffer = nullptr;
		Ref<IndexBuffer> m_indexBuffer = nullptr;


	};




	class ParticleSpriteRendererComponent : public RendererComponent
	{
	public:
		~ParticleSpriteRendererComponent() = default;
		ParticleSpriteRendererComponent()
		{
			RD_CORE_INFO(" ParticleSpriteRendererComponent created:"); 
		}

		static Ref<ParticleSpriteRendererComponent> Create()
		{
			return CreateRef<ParticleSpriteRendererComponent>();
		}

	public:
		virtual void Draw(Ref<Camera> cam) override;
		virtual void DrawInstanced(Ref<Camera> cam, uint32_t count) override;
  

	};







}