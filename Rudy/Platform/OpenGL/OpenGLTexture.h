#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Texture.h"
#include <stb_image.h>

#include <glad/glad.h>

//todo: 
// organinze properties in a struct, so that we won't need to change class-by-class.
//custom mipmap LOD ;


namespace Rudy {

	  
	class OpenGLTexture2D : public Texture2D
	{
	public:
		~OpenGLTexture2D();
		OpenGLTexture2D(const std::string& path, bool isHDRI);
		OpenGLTexture2D(const TextureSpec& specification); 


		//======inherited

		virtual void Bind(uint32_t slot = 0) const override
		{
			glBindTextureUnit(slot, m_TextureID);
		}
		virtual void Unbind(uint32_t slot = 0) const override
		{
			glBindTextureUnit(slot, 0);
		}

		virtual void SubData(void* data,
			uint32_t width, uint32_t height, uint32_t xOffset = 0, uint32_t yOffset = 0) override;
		

		virtual bool IsLoaded() const override { return m_IsLoaded; }


		//======getters&setters======	
		//virtual void SetData(void* data, uint32_t size) override;

		virtual const TextureSpec& GetTextureSpec() const override { return m_TextureSpec; }  
		virtual uint32_t GetWidth() const override { return m_Width;  }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetID() const override { return m_TextureID; }
		virtual const std::string& GetPath() const override { return m_Path; }  
	
	 
		

	public: 
		uint32_t m_TextureID;

		TextureSpec m_TextureSpec;
 
		//stbi_uc* m_TextureData;  
		//usually no need, data on memory is released after transfer to GPU
		//unless in the future we want to do some cpu side thing, like preview, processing, etc

		uint32_t m_Width, m_Height;
		std::string m_Path;
		 
		GLenum m_InternalFormat, m_DataFormat, m_DataType;
		GLenum m_WrapMode, m_MinFilterMode, m_MagFilterMode;

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

		virtual void SubData(void* data,
			uint32_t width, uint32_t height, uint32_t xOffset = 0, uint32_t yOffset = 0) override
		{ //to be implemented
			RD_CORE_ERROR( "Not implemented yet");}


		virtual bool IsLoaded() const override { return m_IsLoaded; }

 

		//======getters&setters======	
		//virtual void SetData(void* data, uint32_t size) override;

		virtual const TextureSpec& GetTextureSpec() const override { return m_TextureSpec; }
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetID() const override { return m_TextureID; }
		virtual const std::string& GetPath() const override { return m_Path; }



		Ref<TextureCube> CreatePrefilteredEnvMap(Ref<TextureCube> envMap,
			ConvolutionType type, uint32_t mipLevels) override;

		//virtual bool operator==(const Texture& other) const override
		//{
		//	return m_TextureID == other.GetID();
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

		GLenum m_InternalFormat, m_DataFormat, m_DataType;
		GLenum m_WrapMode, m_MinFilterMode, m_MagFilterMode;

		bool m_IsLoaded = false;

	};




}


 