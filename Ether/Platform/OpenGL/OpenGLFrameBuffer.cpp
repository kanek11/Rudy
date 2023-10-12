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


	OpenGLFrameBuffer::OpenGLFrameBuffer(uint32_t width, uint32_t height, FrameBufferType type, std::unordered_map<TextureType, Ref<Texture>> textureBuffers)
		: m_TextureBuffers(textureBuffers) 
	{


	  switch (type)
	  {
		case FrameBufferType::Default:
		{
			//create and bind framebuffer
			glCreateFramebuffers(1, &m_FrameBufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);


			//1 colorbuffer
			if (m_TextureBuffers.size() == 0)
			{
				HZ_CORE_ERROR("glframebuffer default�� No colorbuffer is attached to framebuffer,create itself ");

				//m_TextureBuffers[(TextureType)(0)] = Texture2D::CreateEmpty(TextureSpec{width,height});
			}


			//attach colorbuffer to framebuffer
			for (auto g_texture : m_TextureBuffers)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_texture.second->GetTextureID(), 0);
				HZ_CORE_INFO("glframebuffer default: textureType:{0} with id:{1} is attached", (int)g_texture.first, g_texture.second->GetTextureID());
			}
		 

			//depthbuffer
			m_RenderBuffer = RenderBuffer::Create( width, height, RenderBufferFormat::DEPTH24STENCIL8);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer->GetRenderBufferID());



			//wrapping up
			//check framebuffer status
			HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			//unbind framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			HZ_CORE_INFO("Default FrameBufferID:{0} is created  ", m_FrameBufferID);

			break;

		}


		case FrameBufferType::DepthMap:
		{

			//create and bind framebuffer
			glCreateFramebuffers(1, &m_FrameBufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);


			//texture for depth map
			if (m_TextureBuffers.size() == 0)
			{
				HZ_CORE_ERROR("glframebuffer depth�� No texture is attached to framebuffer, create itself ");
				//m_TextureBuffers[TextureType::DepthMap] =
				//	Texture2D::CreateEmpty(TextureSpec{ width, height,TextureFormat::DEPTH_COMPONENT,
				//	 false, WrapMode::ClampToBorder, FilterMode::Nearest });

			}

			m_TextureBuffers[TextureType::DepthMap]->Bind();
			//m_TextureBuffers.push_back(Texture2D::Create(
			//	TextureSpec{ width,height, }));

			//special needs for depth map.
			//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			float borderColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


			//attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureBuffers[TextureType::DepthMap]->GetTextureID(), 0);

			//disable color buffer
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			//donot check framebuffer status

			//unbind framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			HZ_CORE_WARN("DepthMap FrameBufferid:{0} is created", m_FrameBufferID);

			break;

		}

		case FrameBufferType::GBuffer:
		{
			//create and bind framebuffer
			glCreateFramebuffers(1, &m_FrameBufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);


			uint32_t slot = 0;
			std::vector<uint32_t> attachments;

			if (m_TextureBuffers.size() == 0)
			{
				HZ_CORE_ERROR("glframebuffer GBuffer�� No texture is attached to Gbuffer ");
			}

			for (auto g_texture : m_TextureBuffers)
			{
				//for depth map;
				if (g_texture.first == TextureType::DepthMap)
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_texture.second->GetTextureID(), 0);

				}
				else 
				{
					//attach texture to colorbuffers
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, g_texture.second->GetTextureID(), 0);
					attachments.push_back(GL_COLOR_ATTACHMENT0 + slot);
					HZ_CORE_INFO("GBuffer: textureId:{0} is attached to colorbuffer{1}", g_texture.second->GetTextureID(), slot);
					slot++;
				}

				
			} 

			glDrawBuffers(attachments.size(), attachments.data()); 
			
			//output the attachments:
			//for (int i = 0; i < attachments.size(); i++)
			//{
			//	HZ_CORE_INFO("GBuffer: colorbuffer:{0} is set as drawbuffers", attachments[i]);
			//}
		

			//depthbuffer
			m_RenderBuffer = RenderBuffer::Create( width, height, RenderBufferFormat::DEPTH24STENCIL8);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer->GetRenderBufferID());

			 
			//wrapping up
		   //check framebuffer status
			HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			//unbind framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			HZ_CORE_WARN("GBuffer id:{0} is created, buffer size :{1} ", m_FrameBufferID, m_TextureBuffers.size());

			break;

		}
		 

	  }
	}

}
	 