
#include "EtherPCH.h"  
 
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Hazel/Renderer/Material.h"
#include "Cube.h"

#include <stb_image.h>
 

namespace Hazel {



	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	//glm::mat4 captureViews[] = {
	//	// Right face (Positive X)
	//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	//	// Left face (Negative X)
	//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	//	// Top face (Positive Y)
	//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
	//	// Bottom face (Negative Y)
	//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
	//	// Front face (Positive Z)
	//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	//	// Back face (Negative Z)
	//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
	//};


	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};




	namespace Utils {

		static GLenum TextureFormatToGLFormat(TextureFormat format)
		{
			switch (format)
			{ 
		    case TextureFormat::R8: return GL_R8; 
			case TextureFormat::R32F: return GL_R32F;
		    case TextureFormat::RG8: return GL_RG8;
		 case TextureFormat::RG32F: return GL_RG32F;
			case TextureFormat::RGB8:  return GL_RGB8; 
			case TextureFormat::RGB32F: return GL_RGB32F;
			case TextureFormat::RGBA8: return GL_RGBA8;
			case TextureFormat::RGBA32F: return GL_RGBA32F;

			case TextureFormat::DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
			}

			HZ_CORE_ASSERT(false);
			return 0;
		}


		static  GLenum TextureFormatToDataFormat(TextureFormat format)
		{
			switch (format)
			{ 
			case TextureFormat::R8: return GL_RED; 
			case TextureFormat::R32F: return GL_RED;
			case TextureFormat::RG8: return GL_RG;
		    case TextureFormat::RG32F: return GL_RG;
			case TextureFormat::RGB8:  return GL_RGB;
			case TextureFormat::RGB32F: return GL_RGB;
			case TextureFormat::RGBA8: return GL_RGBA;
		    case TextureFormat::RGBA32F: return GL_RGBA;

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
			case FilterMode::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
			}

			HZ_CORE_ASSERT(false);
			return 0;
		}

	}



	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool isHDRI)
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
			HZ_CORE_WARN("glTexture2D: failded to load texture at path: {0}", path.c_str());
		}
		else
		{ 
			m_IsLoaded = true;

			m_Width = width;
			m_Height = height;

			GLenum internalFormat = 0, dataFormat = 0;

			if (isHDRI)
			{ 
				internalFormat = GL_RGB16F;
				dataFormat = GL_RGB;
			}
		    else
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
			else if (channels == 1)
			{
				internalFormat = GL_R8;
				dataFormat = GL_RED;
			}

			auto m_InternalFormat = internalFormat;
			auto m_DataFormat = dataFormat;

			HZ_CORE_ASSERT(internalFormat & dataFormat, "glTexture:Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

			//glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

			 glTextureStorage2D(m_TextureID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			 glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);


			HZ_CORE_WARN("glTexture2D:texture2DId:{0} loaded at path:{1}", m_TextureID, path.c_str());
			HZ_CORE_WARN("glTexture2D:width:{0},height:{1}, channels:{2}", m_Width,m_Height, channels);

			
		}
	}

	 


	OpenGLTexture2D::OpenGLTexture2D(const TextureSpec& specification)
		: m_TextureSpec(specification), m_Width(m_TextureSpec.Width), m_Height(m_TextureSpec.Height)
	{

		//HZ_PROFILE_FUNCTION();

		//deny to create texture with 0 size
		HZ_CORE_ASSERT(m_Width && m_Height, "Texture width and height must be greater than 0!");


		auto m_InternalFormat = Utils::TextureFormatToGLFormat(specification.TextureFormat);
		auto m_DataFormat = Utils::TextureFormatToDataFormat(specification.TextureFormat); 
		auto m_WrapMode = Utils::WrapModeToGLWrapMode(specification.wrapMode);
		auto m_MinFilterMode = Utils::FilterModeToGLFilterMode(specification.minfilterMode); 
		auto m_MagFilterMode = Utils::FilterModeToGLFilterMode(specification.magfilterMode);


		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);


		 //glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		//lTextureStorage2D(m_TextureID, 1, m_InternalFormat, m_Width, m_Height);

	    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_FLOAT, NULL);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, m_MinFilterMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, m_MagFilterMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, m_WrapMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, m_WrapMode);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		  
	      HZ_CORE_WARN("glTexture2D: empty texture2DId:{0} is created ", m_TextureID); 

	}




	OpenGLTexture2D::OpenGLTexture2D(const TextureSpec& specification, void* data)
		: m_TextureSpec(specification), m_Width(m_TextureSpec.Width), m_Height(m_TextureSpec.Height)
	{

		//HZ_PROFILE_FUNCTION();

		//deny to create texture with 0 size
		HZ_CORE_ASSERT(m_Width && m_Height, "Texture width and height must be greater than 0!");


		auto m_InternalFormat = Utils::TextureFormatToGLFormat(specification.TextureFormat);
		auto m_DataFormat = Utils::TextureFormatToDataFormat(specification.TextureFormat);
		auto m_WrapMode = Utils::WrapModeToGLWrapMode(specification.wrapMode);
		auto m_MinFilterMode = Utils::FilterModeToGLFilterMode(specification.minfilterMode);
		auto m_MagFilterMode = Utils::FilterModeToGLFilterMode(specification.magfilterMode);


		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);


		//glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
	   //lTextureStorage2D(m_TextureID, 1, m_InternalFormat, m_Width, m_Height);

		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_FLOAT, data);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, m_MinFilterMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, m_MagFilterMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, m_WrapMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, m_WrapMode);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		HZ_CORE_WARN("glTexture2D: custom texture2DId:{0} is created ", m_TextureID);

	}









	 
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		//HZ_PROFILE_FUNCTION();
		HZ_CORE_WARN("texture2D: texture2DID:{0} is deleted ", m_TextureID);
		glDeleteTextures(1, &m_TextureID);
	}


 

	//void OpenGLTexture2D::SetData(void* data, uint32_t size)
	//{
	//	//HZ_PROFILE_FUNCTION();
	//
	//	uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
	//	HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
	//	glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	//}


	//===========Cube



	//OpenGLTextureCube(const std::string& path);
	//OpenGLTextureCube(const std::vector<std::string>& paths);
	//OpenGLTextureCube(const TextureSpec& specification);



	OpenGLTextureCube::OpenGLTextureCube(const TextureSpec& specification)
		: m_TextureSpec(specification), m_Width(m_TextureSpec.Width), m_Height(m_TextureSpec.Height)
	{

		//HZ_PROFILE_FUNCTION();

		//deny to create texture with 0 size
		HZ_CORE_ASSERT(m_Width && m_Height, "Texture width and height must be greater than 0!");

		 
		auto m_InternalFormat = Utils::TextureFormatToGLFormat(specification.TextureFormat);
		auto m_DataFormat = Utils::TextureFormatToDataFormat(specification.TextureFormat); 
		auto m_WrapMode = Utils::WrapModeToGLWrapMode(specification.wrapMode);
		auto m_MinFilterMode = Utils::FilterModeToGLFilterMode(specification.minfilterMode);
		auto m_MagFilterMode = Utils::FilterModeToGLFilterMode(specification.magfilterMode);



		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_FLOAT, NULL);
		}

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, m_MinFilterMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, m_MagFilterMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, m_WrapMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, m_WrapMode);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_R, m_WrapMode);


		if (specification.GenerateMips)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}
		 

		HZ_CORE_WARN("textureCube: CubeMapId:{0} is created ", m_TextureID);

		 

	}


	OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string>& paths)
	{
		//HZ_PROFILE_FUNCTION();

		
        //opengl assume uv of bottom left origin and stbi is top left origin
		stbi_set_flip_vertically_on_load(Texture::s_FlipYOnLoad); 

		//initial check
		HZ_CORE_ASSERT(paths.size() == 6, "TextureCube must have 6 paths");

		 
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);


		int width, height, channels;

		//load for 6 times.
		for (unsigned int i = 0; i < paths.size(); i++)
		{

			stbi_uc* data = nullptr;
			{
				//HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D(const std::string&)");
				data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
			}
            if (!data)
		    {
		        HZ_CORE_WARN("OpenGLTexture: failded to load texture at path: {0}", paths[i].c_str());
		    }
			else
			{
				HZ_CORE_WARN("OpenGLTexture: load texture at path: {0}", paths[i].c_str());

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
			 
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
				   
				stbi_image_free(data);
 
			} 

		}


		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		 
		 
		HZ_CORE_WARN("textureCube: CubeMapID:{0} is created " , m_TextureID);
	}



	//convert hdri image to cubemap
	OpenGLTextureCube::OpenGLTextureCube(const std::string& path)
	{
		//HZ_PROFILE_FUNCTION();

		//opengl assume uv of bottom left origin and stbi is top left origin
		stbi_set_flip_vertically_on_load(Texture::s_FlipYOnLoad);

		  
		//input hdri image
		auto hdrTexture = Texture2D::LoadFile(path);


		//output  
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
		
		HZ_CORE_WARN("textureCube: CubeMapID:{0} is created ", m_TextureID);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		 

		//shader
		auto rectToCubeShader = Shader::Create("equirectangular to cubemap Shader", "Resources/Shaders/RectToCube_VS.glsl", "Resources/Shaders/RectToCube_FS.glsl");
		Material::SetMaterialProperties(rectToCubeShader); 
	    
		//material 
		auto rectToCubeMaterial = Material::Create(rectToCubeShader);
		rectToCubeMaterial->SetTexture(TextureType::EnvironmentMap, hdrTexture);


		Cube cube;
		cube.SetMaterial(rectToCubeMaterial); 


		unsigned int captureFBO;
		glGenFramebuffers(1, &captureFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);


		unsigned int captureRBO;
		glGenRenderbuffers(1, &captureRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		 
 
		glViewport(0, 0, 512, 512); 
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		for (unsigned int i = 0; i < 6; ++i)
		{
			rectToCubeShader->Bind();
			rectToCubeShader->SetMat4("u_ProjectionView", captureProjection * captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_TextureID, 0); 

			glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);


			cube.Draw();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
 

	}


	//create a identity matrix

		//rectToCubeShader->SetMat4("u_ProjectionView", identity);


	Ref<TextureCube> OpenGLTextureCube::CreatePrefilteredEnvMap(Ref<TextureCube> envMap, ConvolutionType type, uint32_t mipLevels)
	{
		//HZ_PROFILE_FUNCTION(); 

		switch (type)
		{
		case ConvolutionType::Specular:
		{

			//initial check
			HZ_CORE_ASSERT(maxMipLevels > 0, "TextureCube: mipLevels must be greater than 0");

			//glGenTextures(1, &m_TextureID);
			//glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
			//
			//HZ_CORE_WARN("textureCube: CubeMapID:{0} is created ", m_TextureID);


			//shader
			auto prefilterShader = Shader::Create("Prefilter specular Shader", "Resources/Shaders/SpecularEnvMap_VS.glsl", "Resources/Shaders/SpecularEnvMap_FS.glsl");
			Material::SetMaterialProperties(prefilterShader);

			//the output prefilter cube map;
			auto prefilterEnvMap = TextureCube::CreateEmpty(TextureSpec{ 128,128,TextureFormat::RGB32F,
			 	  true, WrapMode::ClampToEdge,FilterMode::LinearMipmapLinear, FilterMode::Linear });


			//auto prefilterEnvMap = TextureCube::CreateEmpty(TextureSpec{ 128,128,TextureFormat::RGBA32F,
			//	  false, WrapMode::ClampToEdge,FilterMode::Linear, FilterMode::Linear });


			auto cubeMaterial = Material::Create(prefilterShader);
			cubeMaterial->SetTexture(TextureType::EnvironmentMap, envMap);


			//the background cube;
			Cube cube;
			cube.SetMaterial(cubeMaterial); 


			//framebuffer
			unsigned int captureFBO;
			glGenFramebuffers(1, &captureFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);


			unsigned int captureRBO;
			glGenRenderbuffers(1, &captureRBO);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);



			// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
			// ----------------------------------------------------------------------------------------------------
		 


			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

			//unsigned int maxMipLevels = 5;
			for (unsigned int mip = 0; mip < mipLevels; ++mip)
			{
				// reisze framebuffer according to mip-level size.
				unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
				unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
				glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
				glViewport(0, 0, mipWidth, mipHeight);

				float roughness = (float)mip / ((float)mipLevels - 1);
				prefilterShader->SetFloat("u_Roughness", roughness);

				for (unsigned int i = 0; i < 6; ++i)
				{
					prefilterShader->Bind();
					prefilterShader->SetMat4("u_ProjectionView", captureProjection * captureViews[i]);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterEnvMap->GetTextureID(), mip);

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					cube.Draw();
				}
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return prefilterEnvMap;

		}




		case ConvolutionType::Diffuse:
		{

			auto prefilterShader = Shader::Create("Prefilter diffuse Shader", "Resources/Shaders/DiffuseEnvMap_VS.glsl", "Resources/Shaders/DiffuseEnvMap_FS.glsl");
			Material::SetMaterialProperties(prefilterShader);


			//the output prefilter cube map;
			auto prefilterEnvMap = TextureCube::CreateEmpty(TextureSpec{ 32,32,TextureFormat::RGB32F,
				  false, WrapMode::ClampToEdge, FilterMode::Linear, FilterMode::Linear });


			auto cubeMaterial = Material::Create(prefilterShader);
			cubeMaterial->SetTexture(TextureType::EnvironmentMap, envMap);


			//the background cube;
			Cube cube;
			cube.SetMaterial(cubeMaterial);

			//framebuffer
			unsigned int captureFBO;
			glGenFramebuffers(1, &captureFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);


			unsigned int captureRBO;
			glGenRenderbuffers(1, &captureRBO);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);



			// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
			// ----------------------------------------------------------------------------------------------------
 


			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

			glViewport(0, 0, 32, 32); 
			for (unsigned int i = 0; i < 6; ++i)
			{
				prefilterShader->Bind();
				prefilterShader->SetMat4("u_ProjectionView", captureProjection * captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterEnvMap->GetTextureID(), 0);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				cube.Draw();
			}


			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return prefilterEnvMap;


		}



		}




	}




	OpenGLTextureCube::~OpenGLTextureCube()
	{
		//HZ_PROFILE_FUNCTION();

		HZ_CORE_WARN("textureCube: CubeMapID:{0} is deleted ", m_TextureID);
		glDeleteTextures(1, &m_TextureID);


	}
 

 

 
 
	 


}