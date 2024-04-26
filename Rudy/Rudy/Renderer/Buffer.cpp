
#include "RudyPCH.h"
#include "Buffer.h"

#include "Rudy/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Rudy
{

Ref<VertexBuffer> VertexBuffer::Create()

{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<StorageBuffer> StorageBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLStorageBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<UniformBuffer> UniformBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<VertexArray> VertexArray::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Rudy