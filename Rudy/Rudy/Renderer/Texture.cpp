 

#include "RudyPCH.h"

#include "Rudy/Renderer/Renderer.h"

#include "Rudy/Renderer/Texture.h" 
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Rudy {

	bool Texture::s_FlipYOnLoad = false;

	void Texture::SetFlipYOnLoad(bool flip)
	{
		s_FlipYOnLoad = flip;
	}

	SharedPtr<Texture2D> Texture2D::LoadFile(const std::string& path, bool isHDRI)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateShared<OpenGLTexture2D>(path, isHDRI);
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	
	SharedPtr<Texture2D> Texture2D::CreateEmpty(const TextureSpec& specfication)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateShared<OpenGLTexture2D>(specfication);
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


	 










	SharedPtr<TextureCube> TextureCube::LoadImages(const std::vector<std::string>& paths)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateShared<OpenGLTextureCube>(paths);
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}




	SharedPtr<TextureCube> TextureCube::CreateEmpty(const TextureSpec& specfication)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateShared<OpenGLTextureCube>(specfication);
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}



	 SharedPtr<TextureCube> TextureCube::LoadHDRI(const std::string& path)
	 {
		 switch (RendererAPI::GetAPI())
		 {
		 case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		 case RendererAPI::API::OpenGL:  return CreateShared<OpenGLTextureCube>(path);
		 }

		 RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		 return nullptr;
	 }


	  


}