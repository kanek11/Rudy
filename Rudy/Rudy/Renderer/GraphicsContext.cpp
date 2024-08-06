#include "RudyPCH.h"

#include "Rudy/Renderer/GraphicsContext.h"

#include "Rudy/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Rudy
{

UniquePtr<GraphicsContext> GraphicsContext::Create(void* window)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateUnique<OpenGLContext>(static_cast<GLFWwindow*>(window));
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Rudy