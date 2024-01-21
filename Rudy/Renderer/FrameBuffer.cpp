#include "RudyPCH.h"

#include "FrameBuffer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Rudy/Renderer/Renderer.h"


namespace Rudy {


	Scope<RenderBuffer> RenderBuffer::Create(uint32_t width, uint32_t height, RenderBufferFormat format)

	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRenderBuffer>(format, width, height);
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


	Ref<FrameBuffer> FrameBuffer::Create(std::string name, uint32_t width, uint32_t height, FrameBufferType type)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  
			return CreateRef<OpenGLFrameBuffer>(name, width, height, type);
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}