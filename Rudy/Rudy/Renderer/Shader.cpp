#include "RudyPCH.h"  

#include "Rudy/Renderer/Renderer.h" 

#include "Rudy/Renderer/Shader.h" 
#include "Platform/OpenGL/OpenGLShader.h"


namespace Rudy {

	SharedPtr<Shader> Shader::Create(const std::string& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateShared<OpenGLShader>(filepath);
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	SharedPtr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc,
		const std::string& geometrySrc )
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateShared<OpenGLShader>(name, vertexSrc, fragmentSrc, geometrySrc);
		} 
		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}



	SharedPtr<Shader> Shader::CreateComputeShader(const std::string& name, const std::string& computeSrc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateShared<OpenGLShader>(name, computeSrc);
		}

		RD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}





    std::unordered_map<std::string, SharedPtr<Shader>> ShaderLibrary::s_shaders;

	void ShaderLibrary::Add(const std::string& name, const SharedPtr<Shader>& shader)
	{
		RD_CORE_ASSERT(!Exists(name), "Shader already exists!");
		s_shaders[name] = shader;
	}

	void ShaderLibrary::Add(const SharedPtr<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}


	SharedPtr<Shader> ShaderLibrary::Get(const std::string& name)
	{
		RD_CORE_ASSERT(Exists(name), "Shader not found!");
		return s_shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) 
	{
		return s_shaders.find(name) != s_shaders.end();
	}

}