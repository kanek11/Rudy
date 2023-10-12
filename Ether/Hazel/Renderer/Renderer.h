#pragma once

//#include "Hazel/Renderer/RenderCommand.h"

//#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Renderer/RendererAPI.h"

#include "Hazel/Renderer/Camera.h"
#include "Hazel/Core/Window.h"

#include "Hazel/Renderer/Scene.h"


namespace Hazel {
	 

	class Renderer
	{
	//me: to implement more rendering engines in the future, leave it now.
	enum class RenderingEngine {

		Renderer2D = 0, Renderer3D = 1,
	};


	public:
		static void Init(uint32_t width, uint32_t height);
		static void Shutdown();
		
		static void OnWindowResize(uint32_t width, uint32_t height); 

		static void Render(const Ref<Scene>& scene); 

		//static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); } 
		static Ref<RendererAPI> GetRendererAPI() { return s_RendererAPI; }


		static Ref<Camera> GetMainCamera() { return s_MainCamera; }
		static void SetMainCamera(Ref<Camera> camera) { s_MainCamera = camera; }

		static bool ShouldClose()
		{
			return s_Window->ShouldClose();
		}

		static void WindowOnUpdate()
		{
			s_Window->OnUpdate();
		}


	//static variables
		static Ref<RendererAPI> s_RendererAPI;
		 

	private:
	 
		//camera
		static Ref<Camera> s_MainCamera;

		//the window ;
		static Scope<Window> s_Window;

	 
	};

 
}



//struct SceneData
//{
//	//glm::mat4 View < ProjectionMatrix; 
//};
//
//static Scope<SceneData> s_SceneData;