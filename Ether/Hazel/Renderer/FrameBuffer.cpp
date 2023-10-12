#include "EtherPCH.h"

#include "FrameBuffer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Hazel/Renderer/Renderer.h"


namespace Hazel {


	Scope<RenderBuffer> RenderBuffer::Create(uint32_t width, uint32_t height, RenderBufferFormat format)

	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRenderBuffer>(format, width, height);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


	Ref<FrameBuffer> FrameBuffer::Create(uint32_t width, uint32_t height, FrameBufferType type, std::unordered_map<TextureType, Ref<Texture>> textureBuffers)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLFrameBuffer>(width, height, type, textureBuffers);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}