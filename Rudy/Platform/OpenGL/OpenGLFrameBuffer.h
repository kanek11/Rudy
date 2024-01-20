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

		OpenGLFrameBuffer(std::string name, uint32_t width, uint32_t height, FrameBufferType type);


		virtual void Bind() override { glBindFramebuffer(GL_FRAMEBUFFER, this->m_FrameBufferID); }
		virtual void Unbind() override { glBindFramebuffer(GL_FRAMEBUFFER, 0); } 

		virtual uint32_t GetFrameBufferID()  override { return this->m_FrameBufferID;} 
 


		virtual void SetColorTexture(TexType type, Ref<Texture> texture, uint32_t slot) override
		{
			if (slot >= 8)
			{
				RD_CORE_ERROR("framebuffer: colorbuffer slot out of range!");
				return;
			}
			 
			m_TextureBuffers[type] = texture;
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, texture->GetID(), 0);	
			glNamedFramebufferTexture(this->m_FrameBufferID, GL_COLOR_ATTACHMENT0 + slot, 
				                       texture->GetID(), 0);
			
			RD_CORE_INFO("GBuffer: textureId:{0} is attached to colorbuffer{1}", texture->GetID(), slot);
	 
		}

		virtual void SetDepthTexture(Ref<Texture> texture) override
		{
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->GetID(), 0);
			 
			m_TextureBuffers[TexType::DepthTexture] = texture;
			glNamedFramebufferTexture(this->m_FrameBufferID, GL_DEPTH_ATTACHMENT, texture->GetID(), 0);
		}


		virtual Ref<Texture> GetTextureByType(TexType type) override
		{
			if (m_TextureBuffers.find(type) == m_TextureBuffers.end())
			{
				RD_CORE_ERROR("gl framebuffer: texture type not found!");
				return nullptr;
			}
			return m_TextureBuffers[type]; 
		} 
	

		virtual void SetDrawBuffers() override
		{
			//size of m_TextureBuffer
			m_ColorBufferNum = m_TextureBuffers.size();

			if(m_ColorBufferNum == 0)
			{
				RD_CORE_WARN(" no colorbuffer attrachment! be sure it's depth mapping fbo");
				glNamedFramebufferDrawBuffer(this->m_FrameBufferID, GL_NONE);
				return;
			} 

			std::vector<uint32_t> colorBuffers;
			for (unsigned int slot = 0; slot < m_ColorBufferNum; slot++)
			{
				colorBuffers.push_back(GL_COLOR_ATTACHMENT0 + slot);
			}  
			glNamedFramebufferDrawBuffers(this-> m_FrameBufferID, m_ColorBufferNum, colorBuffers.data() );
		
		} 


		virtual void CheckCompleteness() override
		{
		    RD_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		}

		virtual void FinishSetup() override
		{ 
			this->SetDrawBuffers();
			this->CheckCompleteness();
		}


		 
	private:
		std::string m_Name;
		uint32_t m_FrameBufferID = 0;  
		uint32_t m_ColorBufferNum = 0; 
		uint32_t m_Width = 0, m_Height = 0;

		std::unordered_map<TexType, Ref<Texture>>  m_TextureBuffers;
		Scope<RenderBuffer> m_RenderBuffer;   //only for depth/stencil  
		 

	};









}