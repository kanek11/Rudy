#pragma once
#include "EtherPCH.h"

#include "Hazel/Renderer/FrameBuffer.h"
#include <glad/glad.h>


namespace Hazel {



	class OpenGLRenderBuffer : public RenderBuffer
	{
	public:

		~OpenGLRenderBuffer() = default;
		OpenGLRenderBuffer(RenderBufferFormat format, uint32_t width, uint32_t height);

		virtual uint32_t GetRenderBufferID() override { return m_RenderBufferID;}

		static Scope<RenderBuffer> Create(RenderBufferFormat format, uint32_t width, uint32_t height);

	private:
		uint32_t m_Width = 0, m_Height = 0;
		uint32_t m_RenderBufferID = 0;
		RenderBufferFormat m_Format = RenderBufferFormat::DEPTH24STENCIL8;


	};






	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		~OpenGLFrameBuffer() = default;

		OpenGLFrameBuffer(FrameBufferSpec& spec);
		OpenGLFrameBuffer(uint32_t width, uint32_t height, FrameBufferType type);


		virtual void Bind() override { glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID); }
		virtual void Unbind() override { glBindFramebuffer(GL_FRAMEBUFFER, 0); }


		virtual uint32_t GetFrameBufferID()  override { return m_FrameBufferID;}
		virtual uint32_t GetTextureBufferID(uint32_t index)  override { return m_TextureBuffers[index]->GetTextureID(); }
		virtual uint32_t GetRenderBufferID()  override { return m_RenderBuffer->GetRenderBufferID(); }

		 
	private:

		uint32_t m_FrameBufferID = 0;
		FrameBufferSpec m_Spec;

		std::vector<Ref<Texture2D>> m_TextureBuffers; //readable textures
		Scope<RenderBuffer> m_RenderBuffer;   //renderbuffer
 
		 

	};









}