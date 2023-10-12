//#include "hzpch.h"
//

#include "EtherPCH.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Hazel {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		HZ_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		//HZ_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");

		HZ_CORE_INFO("glcontext: OpenGL Info:");
		HZ_CORE_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		HZ_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		HZ_CORE_INFO("Version: {0}", glGetString(GL_VERSION));


		GLint maxDrawBuffers;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
		HZ_CORE_INFO("MRT: Max supported draw buffers: {0}", maxDrawBuffers);  

		HZ_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "required at least OpenGL version 4.5");
	}

	void OpenGLContext::SwapBuffers()
	{
		//HZ_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}
