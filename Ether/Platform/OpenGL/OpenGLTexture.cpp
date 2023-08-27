
#include "EtherPCH.h"  
 
#include "Platform/OpenGL/OpenGLTexture.h"
#include <stb_image.h>
 

namespace Hazel {

	namespace Utils {

		static GLenum TextureFormatToGLFormat(TextureFormat format)
		{
			switch (format)
			{

			case TextureFormat::RGB:  return GL_RGB; 
			case TextureFormat::RGB8:  return GL_RGB8;
			case TextureFormat::RGBA8: return GL_RGBA;

			case TextureFormat::DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
			}

			HZ_CORE_ASSERT(false);
			return 0;
		}
 

		static GLenum WrapModeToGLWrapMode(WrapMode mode)
		{
			switch (mode)
			{
			case WrapMode::Repeat: return GL_REPEAT;
			case WrapMode::ClampToEdge  :  return GL_CLAMP_TO_EDGE;
			case WrapMode::ClampToBorder : return GL_CLAMP_TO_BORDER;
			}

			HZ_CORE_ASSERT(false);
			return 0;
		}

		static GLenum FilterModeToGLFilterMode(FilterMode mode)
		{
			switch (mode)
			{
			case FilterMode::Linear:  return GL_LINEAR;
			case FilterMode::Nearest: return GL_NEAREST;
			}

			HZ_CORE_ASSERT(false);
			return 0;
		}

	}



	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		//HZ_PROFILE_FUNCTION();
 

		int width=0, height=0, channels=0;

		//in most case, set flip vertically on load for opengl,  
		//opengl assume uv of bottom left origin and stbi is top left origin
		stbi_set_flip_vertically_on_load(Texture::s_FlipYOnLoad);


		stbi_uc* data = nullptr;
		{ 
			//HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D(const std::string&)");
       
			 data = stbi_load(path.c_str(), &width, &height, &channels, 0); 
		}


		if (!data)
		{
			HZ_CORE_WARN("OpenGLTexture: failded to load texture at path: {0}", path.c_str());
		}
		else
		{
	
			m_IsLoaded = true;

			m_Width = width;
			m_Height = height;

			GLenum internalFormat = 0, dataFormat = 0;
			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			m_InternalFormat = internalFormat;
			m_DataFormat = dataFormat;

			HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
			glTextureStorage2D(m_TextureID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);


			HZ_CORE_WARN("OpenGLTexture:textureid:{0} loaded and setup at path:{1}", m_TextureID, path.c_str());

			
		}
	}





	OpenGLTexture2D::OpenGLTexture2D(const TextureSpec& specification)
		: m_TextureSpec(specification), m_Width(m_TextureSpec.Width), m_Height(m_TextureSpec.Height)
	{

		//HZ_PROFILE_FUNCTION();

		//deny to create texture with 0 size
		HZ_CORE_ASSERT(m_Width && m_Height, "Texture width and height must be greater than 0!");


		m_InternalFormat = Utils::TextureFormatToGLFormat(specification.TextureFormat);
		m_DataFormat = Utils::TextureFormatToGLFormat(specification.TextureFormat);
		m_WrapMode = Utils::WrapModeToGLWrapMode(specification.wrapMode);
		m_FilterMode = Utils::FilterModeToGLFilterMode(specification.filterMode);
		 

		//glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		//glTextureStorage2D (m_TextureID, 1, m_InternalFormat, m_Width, m_Height);
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_InternalFormat, GL_FLOAT, NULL);

		 
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, m_FilterMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, m_FilterMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, m_WrapMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, m_WrapMode); 
  
 

		HZ_CORE_WARN("Debug: OpenGLTexture:textureid:{0} is created ", m_TextureID);


	}










	OpenGLTexture2D::~OpenGLTexture2D()
	{
		//HZ_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		//HZ_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		//HZ_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_TextureID);
	}

	void OpenGLTexture2D::Unbind(uint32_t slot) const
	{
		//HZ_PROFILE_FUNCTION();

		glBindTextureUnit(slot, 0);
	}

}