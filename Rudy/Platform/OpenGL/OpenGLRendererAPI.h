#pragma once

#include "Rudy/Renderer/Camera.h"
#include "Rudy/Renderer/RendererAPI.h"

//cpp
#include "RudyPCH.h"

#include "Rudy/Renderer/Renderer.h"
//#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>



namespace Rudy {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
        virtual void SetClearColor(float r, float g, float b, float a) override;
		virtual void Clear() override;

		virtual void DrawElement(uint32_t indexCount, MeshTopology topo) override;
		virtual void DrawArray(uint32_t vertexCount, MeshTopology topo) override;



	/*	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
		
		virtual void SetLineWidth(float width) override;*/
	};


}



