// #include "hzpch.h"

#include "EtherPCH.h"

#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Hazel
{

	//void OpenGLRendererAPI::DrawElements(const Ref<Mesh> &mesh, const Ref<Material> &material, Transform transform)
	void OpenGLRendererAPI::DrawElements(uint32_t indexCount)
	{ 
		//HZ_PROFILE_FUNCTION(); 
		// 
		// glDrawElements(GL_TRIANGLE_STRIP, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		// for now!
 
	}

	void OpenGLRendererAPI::DrawArrays(uint32_t vertexCount)
	{
		//HZ_PROFILE_FUNCTION();

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
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
			HZ_CORE_CRITICAL(message);
			return;
		case GL_DEBUG_SEVERITY_MEDIUM:
			HZ_CORE_ERROR(message);
			return;
		case GL_DEBUG_SEVERITY_LOW:
			HZ_CORE_WARN(message);
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			HZ_CORE_TRACE(message);
			return;
		}

		HZ_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		//HZ_PROFILE_FUNCTION();

#ifdef HZ_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
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