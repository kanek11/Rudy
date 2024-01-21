#include "RudyPCH.h"

#include "OpenGLFrameBuffer.h"



namespace Rudy {
	 
	//Utility function to convert formats 
	namespace Utils {

		static GLenum RenderBufferFormatToGLFormat(RenderBufferFormat format)
		{
			switch (format)
			{
			case RenderBufferFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			case RenderBufferFormat::DEPTH_COMPONENT24: return GL_DEPTH_COMPONENT24;
			case RenderBufferFormat::STENCIL_INDEX8: return GL_STENCIL_INDEX8;
			}

			RD_CORE_ASSERT(false, "Unknown TextureInternalFormat!");
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


	OpenGLFrameBuffer::OpenGLFrameBuffer(std::string name, uint32_t width, uint32_t height, FrameBufferType type)
		: m_Name(name), m_Width(width), m_Height(height) 
	{
		switch (type)
		{
		case FrameBufferType::Regular:
		{
			//create and bind framebuffer
			glCreateFramebuffers(1, &m_FrameBufferID);

			//depthbuffer
			m_RenderBuffer = RenderBuffer::Create(width, height, RenderBufferFormat::DEPTH24STENCIL8);
			glNamedFramebufferRenderbuffer(this->m_FrameBufferID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer->GetRenderBufferID());

			RD_CORE_INFO("Regular FrameBuffer {0} ,ID:{1} is created", m_Name, m_FrameBufferID);

			break;

		}


		case FrameBufferType::DepthTexture:
		{

			//create and bind framebuffer
			glCreateFramebuffers(1, &m_FrameBufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

			//special needs for depth map.
			//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 

			//disable color buffer
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			//donot check framebuffer status

			//unbind framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			RD_CORE_INFO("DepthTexture FrameBufferid:{0} is created", m_FrameBufferID);

			break;

		}

		case FrameBufferType::GBuffer:
		{
			//create and bind framebuffer
			glCreateFramebuffers(1, &m_FrameBufferID);

			RD_CORE_INFO("GBuffer id:{0} is created, color buffer size :{1} ", m_FrameBufferID, m_ColorBufferNum);

			break;

		}


		}
    }


}



 