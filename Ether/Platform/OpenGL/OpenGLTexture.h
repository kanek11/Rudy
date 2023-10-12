#pragma once

#include "EtherPCH.h"

#include "Hazel/Renderer/Texture.h"
#include <stb_image.h>

#include <glad/glad.h>

namespace Hazel {

	 


	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(const TextureSpec& specification);
	    ~OpenGLTexture2D();


		//gl utils
		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind(uint32_t slot = 0) const override;
		

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
	private:

		//inherited
		TextureSpec m_TextureSpec;
 
		//stbi_uc* m_TextureData;  
		//usually no need, data on memory is released after transfer to GPU
		//unless in the future we want to do some cpu side thing, like preview, processing, etc

		uint32_t m_Width, m_Height;
		uint32_t m_TextureID; 
		std::string m_Path;

        //not to be queried from outside, no getter for now.  
		GLenum m_InternalFormat, m_DataFormat; 
		GLenum m_WrapMode, m_FilterMode;

		bool m_IsLoaded = false;
 
	};

	 

	class OpenGLTextureCube : public TextureCube
	{
	public:

		~OpenGLTextureCube();

		OpenGLTextureCube(const std::string& path) {};  //TODO: implement
		OpenGLTextureCube(const TextureSpec& specification);   


		OpenGLTextureCube(const std::vector<std::string>& paths);
		OpenGLTextureCube(const std::string& path, uint32_t mipLevels);

		//gl utils
		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind(uint32_t slot = 0) const override;


		virtual bool IsLoaded() const override { return m_IsLoaded; }


		//======getters&setters======	
		//virtual void SetData(void* data, uint32_t size) override;

		virtual const TextureSpec& GetTextureSpec() const override { return m_TextureSpec; }
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetTextureID() const override { return m_TextureID; }
		virtual const std::string& GetPath() const override { return m_Path; }



		//virtual bool operator==(const Texture& other) const override
		//{
		//	return m_TextureID == other.GetTextureID();
		//}
	private:

		//inherited
		TextureSpec m_TextureSpec;

		//stbi_uc* m_TextureData;  
		//usually no need, data on memory is released after transfer to GPU
		//unless in the future we want to do some cpu side thing, like preview, processing, etc

		uint32_t m_Width, m_Height;
		uint32_t m_TextureID;
		std::string m_Path;

		//not to be queried from outside, no getter for now.  
		GLenum m_InternalFormat, m_DataFormat;
		GLenum m_WrapMode, m_FilterMode;

		bool m_IsLoaded = false;

	};




}


 