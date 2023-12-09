//#include "hzpch.h"
//

#include "RudyPCH.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Rudy {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		RD_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		//RD_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RD_CORE_ASSERT(status, "Failed to initialize Glad!");

		RD_CORE_INFO("glcontext: OpenGL Info:");
		RD_CORE_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		RD_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		RD_CORE_INFO("Version: {0}", glGetString(GL_VERSION));


		GLint maxDrawBuffers;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
		RD_CORE_INFO("MRT: Max supported draw buffers: {0}", maxDrawBuffers);  

		RD_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "required at least OpenGL version 4.5");
	}

	void OpenGLContext::SwapBuffers()
	{
		//RD_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}
