#include "RudyPCH.h"

#include "RendererAPI.h"
#include "Shader.h"
#include "Runtime/Platform/OpenGL/OpenGLShader.h"

namespace Rudy
{

SharedPtr<Shader> Shader::Create(const std::string& filepath)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<OpenGLShader>(filepath);
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<OpenGLShader>(name, vertexSrc, fragmentSrc, geometrySrc);
    }
    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<Shader> Shader::CreateComputeShader(const std::string& name, const std::string& computeSrc)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<OpenGLShader>(name, computeSrc);
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Rudy