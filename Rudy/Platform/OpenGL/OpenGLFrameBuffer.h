#pragma once
#include "RudyPCH.h"

#include "Rudy/Renderer/FrameBuffer.h"
#include <glad/glad.h>


namespace Rudy {



	class OpenGLRenderBuffer : public RenderBuffer
	{
	public:

		~OpenGLRenderBuffer()
		{
			glDeleteRenderbuffers(1, &m_RenderBufferID);
		}

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
		~OpenGLFrameBuffer()
		{
			 glDeleteFramebuffers(1, &m_FrameBufferID);
		}

		//OpenGLFrameBuffer(FrameBufferSpec& spec);
		OpenGLFrameBuffer(uint32_t width, uint32_t height, FrameBufferType type,
			 uint32_t colorBufferNum);


		virtual void Bind() override { glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID); }
		virtual void Unbind() override { glBindFramebuffer(GL_FRAMEBUFFER, 0); }


		virtual uint32_t GetFrameBufferID()  override { return m_FrameBufferID;}
		virtual uint32_t GetColorBufferNum()  override { return m_ColorBufferNum; }


		//virtual uint32_t GetTextureBufferID(uint32_t index)  override { return m_TextureBuffers[index]->GetTextureID(); }
		//virtual uint32_t GetRenderBufferID()  override { return m_RenderBuffer->GetRenderBufferID(); }

		//virtual void SetTextureBuffers(std::unordered_map<TextureType, Ref<Texture>> TextureBuffers)  override
		//{ m_TextureBuffers = TextureBuffers; }
		//
		//virtual Ref<Texture> GetTextureBufferByType(TextureType type) override 
		//{  if (!m_TextureBuffers[type])
		//     {
		//     	RD_CORE_ERROR("framebuffer: texture type not found!");
		//		return m_TextureBuffers[type];
		//     }
		//   else
		//	return m_TextureBuffers[type];
		//  
		//}
		//
		//virtual std::unordered_map<TextureType, Ref<Texture>> GetTextureBuffers() override 
		//{ return m_TextureBuffers; }



		virtual void SetColorAttachmentTexture(Ref<Texture> texture, uint32_t slot) override
		{
			if (slot > m_ColorBufferNum)
			{
				RD_CORE_ERROR("framebuffer: colorbuffer slot out of range!");
				return;
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, texture->GetTextureID(), 0);
			RD_CORE_INFO("GBuffer: textureId:{0} is attached to colorbuffer{1}", texture->GetTextureID(), slot);
	 
		}
		virtual void SetDepthAttachmentTexture(Ref<Texture> texture) override
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->GetTextureID(), 0);
		}

		virtual void CheckCompleteness() override
		{
		    RD_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		}


		 
	private:

		uint32_t m_FrameBufferID = 0; 
		uint32_t m_ColorBufferNum = 0;


		//FrameBufferSpec m_Spec;

		//std::unordered_map<TextureType, Ref<Texture>>  m_TextureBuffers;
		Scope<RenderBuffer> m_RenderBuffer;   //renderbuffer
 
		 

	};









}