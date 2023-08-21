
//cpp
//#include "hzpch.h"

#include "EtherPCH.h"

#include "Hazel/Renderer/Renderer.h"

//#include "Hazel/Renderer/Renderer2D.h" 
//#include "Hazel/Renderer/Render3D.h"


namespace Hazel {

	//Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	Ref<Camera> Renderer::s_MainCamera;
	Ref<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();

	void Renderer::Init()
	{
		HZ_PROFILE_FUNCTION();

		s_RendererAPI->Init();
		//RenderCommand::Init();
		//Renderer2D::Init();
		//Renderer3D:: Init();
	}

	void Renderer::Shutdown()
	{
		//Renderer2D::Shutdown();
		//Renderer3D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		//RenderCommand::SetViewport(0, 0, width, height);
	}

	//void Renderer::BeginScene(OrthographicCamera& camera)
	void Renderer::BeginScene(Ref<Camera> camera)
	{
		s_MainCamera = camera; 
	}

	void Renderer::EndScene()
	{
	}

	//void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	//{
	//	shader->Bind();
	//	shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
	//	shader->SetMat4("u_Transform", transform);

	//	vertexArray->Bind();
	//	RenderCommand::DrawIndexed(vertexArray);
	//}

}
