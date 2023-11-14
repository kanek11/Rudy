
//cpp
//#include "hzpch.h"

#include "EtherPCH.h"

#include "Hazel/Renderer/Renderer.h" 

//#include "Hazel/Renderer/Renderer2D.h" 
//#include "Hazel/Renderer/Render3D.h"


namespace Hazel {

	//Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	Ref<Camera> Renderer::s_MainCamera = CreateRef<Camera>();
	Scope<Window> Renderer::s_Window ;
	Ref<RendererAPI> Renderer::s_RendererAPI;   

	void Renderer::Init(uint32_t width, uint32_t height)
	{
		//HZ_PROFILE_FUNCTION();
 
		//DONOT touch;
	    RendererAPI::SetAPI( RendererAPI::API::OpenGL );
		s_RendererAPI = RendererAPI::Create();


		s_Window = Window::Create( WindowProps{ width, height });
		Input::SetWindowContext(s_Window->GetNativeWindow());

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


	void Renderer::Render(const Ref<Scene>& scene)
	{
		//HZ_PROFILE_FUNCTION();

		//s_SceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
 

		//loop objects in the scene
		//auto group = scene->m_Registry.group<TransformComponent>(entt::get<MeshComponent>);

		for (auto obj : scene->Objects) 
		{
			//todo:  only for renderable objects
			//obj->Draw();

		}


		//auto view = scene->m_Registry.view<TransformComponent, MeshComponent>();
		//for (auto entity : view)
		//{
		//	auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

		//	Renderer3D::Submit(mesh.mesh, transform.GetTransform());
		//}

	 
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
