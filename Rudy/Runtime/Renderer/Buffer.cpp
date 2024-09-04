
#include "RudyPCH.h"
#include "Buffer.h"

#include "RendererAPI.h"
#include "Runtime/Platform/OpenGL/OpenGLBuffer.h"

namespace Rudy
{

SharedPtr<FRHIVertexBuffer> FRHIVertexBuffer::Create()

{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<FOpenGLVertexBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<FRHIIndexBuffer> FRHIIndexBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<FOpenGLIndexBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<RHIStorageBuffer> RHIStorageBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<FOpenGLStorageBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<FRHIUniformBuffer> FRHIUniformBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<FOpenGLUniformBuffer>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<FRHIVertexArray> FRHIVertexArray::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<FOpenGLVertexArray>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Rudy