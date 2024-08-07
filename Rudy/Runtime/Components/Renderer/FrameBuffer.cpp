#include "RudyPCH.h"

#include "FrameBuffer.h"

#include "Runtime/Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Renderer.h"

namespace Rudy
{

UniquePtr<RenderBuffer> RenderBuffer::Create(uint32_t width, uint32_t height, RenderBufferFormat format)

{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateUnique<OpenGLRenderBuffer>(format, width, height);
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<FrameBuffer> FrameBuffer::Create(std::string name, uint32_t width, uint32_t height, FrameBufferType type)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateShared<OpenGLFrameBuffer>(name, width, height, type);
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Rudy