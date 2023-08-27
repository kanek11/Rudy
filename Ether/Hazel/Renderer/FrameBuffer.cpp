#include "EtherPCH.h"

#include "FrameBuffer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Hazel/Renderer/Renderer.h"


namespace Hazel {


	Scope<RenderBuffer> RenderBuffer::Create(RenderBufferFormat format, uint32_t width, uint32_t height)

	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRenderBuffer>(format, width, height);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


	Ref<FrameBuffer> FrameBuffer::CreateWithSize(uint32_t width, uint32_t height, FrameBufferType type)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLFrameBuffer>(width, height, type);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}