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

		//OpenGLFrameBuffer(FrameBufferSpec& spec);
		OpenGLFrameBuffer(uint32_t width, uint32_t height, FrameBufferType type,
			std::unordered_map<TextureType, Ref<Texture>> textureBuffers);


		virtual void Bind() override { glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID); }
		virtual void Unbind() override { glBindFramebuffer(GL_FRAMEBUFFER, 0); }


		virtual uint32_t GetFrameBufferID()  override { return m_FrameBufferID;}
		//virtual uint32_t GetTextureBufferID(uint32_t index)  override { return m_TextureBuffers[index]->GetTextureID(); }
		//virtual uint32_t GetRenderBufferID()  override { return m_RenderBuffer->GetRenderBufferID(); }

		virtual void SetTextureBuffers(std::unordered_map<TextureType, Ref<Texture>> TextureBuffers)  override
		{ m_TextureBuffers = TextureBuffers; }

		virtual Ref<Texture> GetTextureBufferByType(TextureType type) override 
		{  if (!m_TextureBuffers[type])
		     {
		     	HZ_CORE_ERROR("framebuffer: texture type not found!");
				return m_TextureBuffers[type];
		     }
		   else
			return m_TextureBuffers[type];
		  
		}

		virtual std::unordered_map<TextureType, Ref<Texture>> GetTextureBuffers() override 
		{ return m_TextureBuffers; }

		 
	private:

		uint32_t m_FrameBufferID = 0;
		//FrameBufferSpec m_Spec;

		std::unordered_map<TextureType, Ref<Texture>>  m_TextureBuffers;
		Scope<RenderBuffer> m_RenderBuffer;   //renderbuffer
 
		 

	};









}