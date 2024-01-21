#pragma once

#include "Rudy/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Rudy {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		~OpenGLContext() = default; 

		virtual void Init() override; 

	private:
		GLFWwindow* m_WindowHandle;
	};

}