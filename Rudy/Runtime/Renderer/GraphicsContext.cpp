#include "RudyPCH.h"

#include "GraphicsContext.h"

#include "RendererAPI.h"
#include "Runtime/Platform/OpenGL/OpenGLContext.h"

namespace Rudy
{

UniquePtr<GraphicsContext> GraphicsContext::Create(void* window)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateUnique<OpenGLContext>(static_cast<GLFWwindow*>(window));
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Rudy