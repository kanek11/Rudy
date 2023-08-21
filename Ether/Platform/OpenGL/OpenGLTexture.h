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
		virtual ~OpenGLTexture2D();

		//======getters&setters======	
		virtual void SetData(void* data, uint32_t size) override;

		virtual const TextureProp& GetTextureProp() const override 
		{ return m_TextureProp; }

		virtual uint32_t GetWidth() const override { return m_Width;  }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetTextureID() const override { return m_TextureID; }
		virtual const std::string& GetPath() const override { return m_Path; }


		virtual bool IsLoaded() const override { return m_IsLoaded; } 

		 
		//gl utils
		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind(uint32_t slot = 0) const override;


		//virtual bool operator==(const Texture& other) const override
		//{
		//	return m_TextureID == other.GetTextureID();
		//}
	private:

		//inherited
		TextureProp m_TextureProp;

		//gl specific
		//stbi_uc* m_TextureData;  
		//usually no need, data on memory is released after transfer to gpu
		//unless in the future we want to do some cpu side thing, like preview, processing, etc

		uint32_t m_Width, m_Height;
		uint32_t m_TextureID; 
		std::string m_Path;

		GLenum m_InternalFormat, m_DataFormat; 
		GLenum m_Wrap, m_Filter;

		bool m_IsLoaded = false;
	};

}


 