
#include "RudyPCH.h"
#include "Buffer.h"

#include "Rudy/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Rudy
{

SharedPtr<VertexBuffer> VertexBuffer::Create()

{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<OpenGLVertexBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<IndexBuffer> IndexBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<OpenGLIndexBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<StorageBuffer> StorageBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<OpenGLStorageBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<UniformBuffer> UniformBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<OpenGLUniformBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<VertexArray> VertexArray::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<OpenGLVertexArray>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Rudy