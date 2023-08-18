#pragma once

#include "Hazel/Renderer/Camera.h"
#include "Hazel/Renderer/RendererAPI.h"

//cpp
#include "Ether.h"

#include "Hazel/Renderer/Renderer.h"
//#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>



namespace Hazel {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
        virtual void SetClearColor(float r, float g, float b, float a) override;
		virtual void Clear() override;

		virtual void DrawElements(const Ref<Mesh>& mesh, const Ref<Material>& material, Transform transform) override;


	/*	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
		
		virtual void SetLineWidth(float width) override;*/
	};


}



