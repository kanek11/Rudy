#pragma once

#include "EtherPCH.h"

#include "Hazel/Renderer/Texture.h"
#include <stb_image.h>

#include <glad/glad.h>

namespace Hazel {

	 


	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path, bool isHDRI);
		OpenGLTexture2D(const TextureSpec& specification);
	    ~OpenGLTexture2D();


		//gl utils
		virtual void Bind(uint32_t slot = 0) const override
		{
			glBindTextureUnit(slot, m_TextureID);
		}
		virtual void Unbind(uint32_t slot = 0) const override
		{
			glBindTextureUnit(slot, 0);
		}
		

		virtual bool IsLoaded() const override { return m_IsLoaded; }


		//======getters&setters======	
		//virtual void SetData(void* data, uint32_t size) override;

		virtual const TextureSpec& GetTextureSpec() const override { return m_TextureSpec; }  
		virtual uint32_t GetWidth() const override { return m_Width;  }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetTextureID() const override { return m_TextureID; }
		virtual const std::string& GetPath() const override { return m_Path; }  
	
	

		//virtual bool operator==(const Texture& other) const override
		//{
		//	return m_TextureID == other.GetTextureID();
		//}


		

	public:

		//inherited
		uint32_t m_TextureID;

		TextureSpec m_TextureSpec;
 
		//stbi_uc* m_TextureData;  
		//usually no need, data on memory is released after transfer to GPU
		//unless in the future we want to do some cpu side thing, like preview, processing, etc

		uint32_t m_Width, m_Height;
		std::string m_Path;

        //not to be queried from outside, no getter for now.  
		//GLenum m_InternalFormat, m_DataFormat; 
		//GLenum m_WrapMode, m_FilterMode;

		bool m_IsLoaded = false;
 
	};

	 

	class OpenGLTextureCube : public TextureCube
	{
	public:

		~OpenGLTextureCube();


		OpenGLTextureCube(const TextureSpec& specification);

		OpenGLTextureCube(const std::string& path) ;   
		OpenGLTextureCube(const std::vector<std::string>& paths);

	 
		//gl utils
		virtual void Bind(uint32_t slot = 0) const override
		{
			glBindTextureUnit(slot, m_TextureID);
		}
		virtual void Unbind(uint32_t slot = 0) const override
		{
			glBindTextureUnit(slot, 0);
		}



		virtual bool IsLoaded() const override { return m_IsLoaded; }

 

		//======getters&setters======	
		//virtual void SetData(void* data, uint32_t size) override;

		virtual const TextureSpec& GetTextureSpec() const override { return m_TextureSpec; }
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetTextureID() const override { return m_TextureID; }
		virtual const std::string& GetPath() const override { return m_Path; }



		Ref<TextureCube> CreatePrefilteredEnvMap(Ref<TextureCube> envMap,
			ConvolutionType type, uint32_t mipLevels) override;

		//virtual bool operator==(const Texture& other) const override
		//{
		//	return m_TextureID == other.GetTextureID();
		//}
	public:

		//inherited
		TextureSpec m_TextureSpec;

		//stbi_uc* m_TextureData;  
		//usually no need, data on memory is released after transfer to GPU
		//unless in the future we want to do some cpu side thing, like preview, processing, etc

		uint32_t m_Width, m_Height;
		uint32_t m_TextureID;
		std::string m_Path;

		//not to be queried from outside, no getter for now.  
		//GLenum m_InternalFormat, m_DataFormat;
		//GLenum m_WrapMode, m_FilterMode;

		bool m_IsLoaded = false;

	};




}


 