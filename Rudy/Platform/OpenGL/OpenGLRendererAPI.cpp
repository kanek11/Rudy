// #include "hzpch.h"

#include "RudyPCH.h"

#include "Rudy/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Rudy
{

	//void OpenGLRendererAPI::DrawElements(const Ref<Mesh> &mesh, const Ref<Material> &material, Transform transform)
	void OpenGLRendererAPI::DrawElement(uint32_t indexCount, MeshTopology topo)
	{ 
		//RD_PROFILE_FUNCTION(); 

		switch (topo)
		{
		 case  MeshTopology::TRIANGLES: 
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
			break;

		 case MeshTopology::LINES:
			glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr); 
			break;

		}
	  
	}



	void OpenGLRendererAPI::DrawArray(uint32_t vertexCount, MeshTopology topo)
	{
		//RD_PROFILE_FUNCTION(); 

		switch (topo)
		{
		case  MeshTopology::TRIANGLES:
			glDrawArrays(GL_TRIANGLES, 0, vertexCount); 
			break;

		case MeshTopology::LINES:
			glDrawArrays(GL_LINES, 0, vertexCount); 
			break;

		}

	}



	 


	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char *message,
		const void *userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			RD_CORE_CRITICAL(message);
			return;
		case GL_DEBUG_SEVERITY_MEDIUM:
			RD_CORE_ERROR(message);
			return;
		case GL_DEBUG_SEVERITY_LOW:
			RD_CORE_WARN(message);
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			RD_CORE_TRACE(message);
			return;
		}

		RD_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		//RD_PROFILE_FUNCTION();  
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4 &color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	//{
	//	vertexArray->Bind();
	//	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	//	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	// }

	// void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	//{
	//	vertexArray->Bind();
	//	glDrawArrays(GL_LINES, 0, vertexCount);
	// }

	// void OpenGLRendererAPI::SetLineWidth(float width)
	//{
	//	glLineWidth(width);
	// }

}