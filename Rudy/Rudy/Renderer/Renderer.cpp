 

#include "RudyPCH.h"

#include "Rudy/Renderer/Renderer.h" 

//#include "Rudy/Renderer/Renderer2D.h" 
//#include "Rudy/Renderer/Render3D.h"


namespace Rudy {

	//Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	Ref<Camera> RendererApp::s_MainCamera = CreateRef<Camera>();
	Ref<Window> RendererApp::s_Window ;
	Ref<RendererAPI> RendererApp::s_RendererAPI;   

	void RendererApp::Init(uint32_t width, uint32_t height)
	{
		//RD_PROFILE_FUNCTION();
 
		//fixed dependency of everything: so must go through the renderer for a window
		//camera though, is just a data structure.
	    RendererAPI::SetAPI( RendererAPI::API::OpenGL );

		s_RendererAPI = RendererAPI::Create(); 

		s_Window = Window::Create( WindowProps{ width, height ,"Rudy Engine"});
		Input::SetWindowContext(s_Window->GetNativeWindow());

		s_Window->SetVSync(false);
		 

		RD_CORE_WARN("Renderer: Init");


		//RenderCommand::Init();
		//Renderer2D::Init();
		//Renderer3D:: Init();
	}

	void RendererApp::Shutdown()
	{
		//Renderer2D::Shutdown();
		//Renderer3D::Shutdown();
	}

	void RendererApp::OnWindowResize(uint32_t width, uint32_t height)
	{
		//RenderCommand::SetViewport(0, 0, width, height);
	}


	void RendererApp::Render(const Ref<Scene>& scene)
	{
		//RD_PROFILE_FUNCTION();

		//s_SceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
 

		//loop objects in the scene
		//auto group = scene->m_Registry.group<TransformComponent>(entt::get<MeshComponent>);

		//for (auto obj : scene->Objects) 
		//{
		//	//todo:  only for renderable objects
		//	//obj->Draw();
		//
		//}


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
	//	shader->SetMat4("u_viewProjection", s_SceneData->ViewProjectionMatrix);
	//	shader->SetMat4("u_Transform", transform);

	//	vertexArray->Bind();
	//	RenderCommand::DrawIndexed(vertexArray);
	//}

}
