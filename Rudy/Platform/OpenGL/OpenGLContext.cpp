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

		RD_CORE_INFO("GLContextInit: OpenGL Info:");
		RD_CORE_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		RD_CORE_INFO("Renderer(GPU): {0}", glGetString(GL_RENDERER));
		RD_CORE_INFO("GLVersion: {0}", glGetString(GL_VERSION)); 
		RD_CORE_INFO("GLSL version: {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));	

		  

		GLint maxDrawBuffers;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
		RD_CORE_INFO("MRT: Max supported draw buffers: {0}", maxDrawBuffers);  

		RD_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "required at least OpenGL version 4.5");
	
	
	
	   //new: compute shader info 
		int max_compute_work_group_count[3];
		int max_compute_work_group_size[3];
		int max_compute_work_group_invocations;

		for (int idx = 0; idx < 3; idx++) {
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
		}
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

		RD_CORE_INFO("Compute shader Limitations: ");
		RD_CORE_INFO("maximum number of work groups:{0},{1},{2} ",max_compute_work_group_count[0],  max_compute_work_group_count[1] , max_compute_work_group_count[2]);
		RD_CORE_INFO("maximum local size of a work group: {0},{1},{2} ", max_compute_work_group_size[0], max_compute_work_group_size[1], max_compute_work_group_size[2]);
		RD_CORE_INFO("#invocations in a work group that may be dispatched to a compute shader: {0} ", max_compute_work_group_invocations);

	
	
	

		//some default settings
#ifdef RD_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr); 
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		//be careful;
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);


		//not used;
		//glEnable(GL_MULTISAMPLE); 

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);

		

	
	
	
	}


}
