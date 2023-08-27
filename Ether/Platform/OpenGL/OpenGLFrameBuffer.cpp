#include "EtherPCH.h"

#include "OpenGLFrameBuffer.h"



namespace Hazel {


	//Utility function to convert formats

	namespace Utils {

		static GLenum RenderBufferFormatToGLFormat(RenderBufferFormat format)
		{
			switch (format)
			{
			case RenderBufferFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			}

			HZ_CORE_ASSERT(false, "Unknown TextureFormat!");
			return 0;
		}

	};



	OpenGLRenderBuffer::OpenGLRenderBuffer(RenderBufferFormat format, uint32_t width, uint32_t height)
		: m_Format(format), m_Width(width), m_Height(height)
	{
		//create and bind renderbuffer 
		glCreateRenderbuffers(1, &m_RenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);

		//allocate storage for renderbuffer
		GLenum internalFormat = Utils::RenderBufferFormatToGLFormat(format);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);

		
	}

	//default constructor
	OpenGLFrameBuffer::OpenGLFrameBuffer(uint32_t width, uint32_t height, FrameBufferType type) 
	{
		
		if (type == FrameBufferType::Default)
		{
			
			//create and bind framebuffer
			glCreateFramebuffers(1, &m_FrameBufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);


			//1 colorbuffer
			m_TextureBuffers.push_back(Texture2D::Create(TextureSpec{ width,height }));
 

			//attach colorbuffer to framebuffer
			 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffers[0]->GetTextureID(), 0);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);


			//depthbuffer
			m_RenderBuffer = RenderBuffer::Create(RenderBufferFormat::DEPTH24STENCIL8, width, height);

			//attach renderbuffer to framebuffer
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer->GetRenderBufferID());


			//check framebuffer status
			HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			//unbind framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			HZ_CORE_WARN("Default FrameBufferID:{0} is created  ", m_FrameBufferID);

	     }
		
		else if (type == FrameBufferType::DepthMap)
		{
			
			//create and bind framebuffer
			glCreateFramebuffers(1, &m_FrameBufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);


			//texture for depth map
			 m_TextureBuffers.push_back(Texture2D::Create(
			 	TextureSpec{ width,height, TextureFormat::DEPTH_COMPONENT,
			 	 false, WrapMode::ClampToBorder, FilterMode::Nearest }));

			//m_TextureBuffers.push_back(Texture2D::Create(
			//	TextureSpec{ width,height, }));

			//special needs for depth map.
			//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			float borderColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


			//attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureBuffers[0]->GetTextureID(), 0);

			//disable color buffer
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			//donot check framebuffer status

			//unbind framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			HZ_CORE_WARN("DepthMap FrameBufferid:{0} is created", m_FrameBufferID);


		}


	}


}