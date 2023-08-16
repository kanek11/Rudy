#pragma once

//#include "Hazel/Renderer/RenderCommand.h"

//#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Renderer/RendererAPI.h"

#include "Hazel/Renderer/Camera.h"




namespace Hazel {
	 

	class Renderer
	{
	//me: to implement more rendering engines in the future, leave it now.
	enum class RenderingEngine {

		Renderer2D = 0, Renderer3D = 1,
	};


	public:
		static void Init();
		static void Shutdown();
		
		static void OnWindowResize(uint32_t width, uint32_t height);

		//static void BeginScene(OrthographicCamera& camera);
		static void BeginScene(Camera& camera);
		static void EndScene();

		//static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		

		static Ref<RendererAPI> s_RendererAPI;
		//me��
		//static RenderingEngine GetRenderingEngine() { return s_RenderingEngine; }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope<SceneData> s_SceneData;

		//me:
		
		//static RenderingEngine s_RenderingEngine;
	};

	//me:
	//Renderer::RenderingEngine Renderer::s_RenderingEngine = RenderingEngine::Renderer3D;
}
