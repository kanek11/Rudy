 #include "RudyPCH.h"

#include "Rudy/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Rudy {

	RendererAPI::API RendererAPI::s_API;

	Ref<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}