#pragma once
 
#include "RudyPCH.h"

#include "Rudy/Core/Window.h"


#include "Rudy/Renderer/RendererAPI.h" 
#include "Rudy/Renderer/Camera.h" 
#include "Rudy/Renderer/Scene.h"



namespace Rudy {
	 
	//mainly serve as a state organizer
	//especially the  API, window;
	class RendererApp
	{
	
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
		static Ref<Window> GetWindow() { return s_Window; }

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


	

	private:
		//static variables
		static Ref<RendererAPI> s_RendererAPI; 
		static Ref<Camera> s_MainCamera; 
		static Ref<Window> s_Window;

	 
	};

 
}


 