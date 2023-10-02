 
//#include "hzpch.h"

#include "EtherPCH.h"

#include "Hazel/Renderer/Renderer.h"

#include "Hazel/Renderer/Texture.h" 
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Hazel {

	bool Texture::s_FlipYOnLoad = false;

	void Texture::SetFlipYOnLoad(bool flip)
	{
		s_FlipYOnLoad = flip;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	
	Ref<Texture2D> Texture2D::Create(const TextureSpec& specfication)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(specfication);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}



	Ref<TextureCube> TextureCube::CreateFromImages(const std::vector<std::string>& paths)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTextureCube>(paths);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	 Ref<TextureCube> TextureCube::CreateFromHDRI(const std::string& path)
	 {
		 switch (Renderer::GetAPI())
		 {
		 case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		 case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTextureCube>(path);
		 }

		 HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		 return nullptr;
	 }

	 Ref<TextureCube> TextureCube::CreateEmpty(const TextureSpec& specfication)
	 {
		 switch (Renderer::GetAPI())
		 {
		 case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		 case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTextureCube>(specfication);
		 }

		 HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		 return nullptr;
	 }



}