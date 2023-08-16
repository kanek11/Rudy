#pragma once

//#include "Hazel/Renderer/VertexArray.h"

#include <glm/glm.hpp>



//me:
//usecase:
//abstract class wrap 3D API commands directly used in rendering loop
//include : drawcall, clear, setviewport, setclearcolor, setline width
// 
//define as object functions since it depend on the API type
// 
//to be inherited by the 3D API classes  sth called  "OpenGLRendererAPI" to implement
// 
//define enum of API types
//specify the logic which API is currently used.

//cherno define it using RenderCommand class to wrap as static functions
//i find it not necessary.



namespace Hazel {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual void Clear() = 0;

		//virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		//virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
		
		//virtual void SetLineWidth(float width) = 0;

		static API GetAPI() { return s_API; }
		static Ref<RendererAPI> Create();
	private:
		static API s_API;
	};

}
