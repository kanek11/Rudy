#include "RudyPCH.h"

#include "Rudy/Renderer/GraphicsContext.h"

#include "Rudy/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Rudy {


	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


}