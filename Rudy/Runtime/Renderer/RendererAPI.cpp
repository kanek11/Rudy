#include "RudyPCH.h"

#include "RendererAPI.h"
#include "Runtime/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Rudy
{

RendererAPI::API RendererAPI::s_API;

SharedPtr<RendererAPI> RendererAPI::Create()
{
    switch (s_API)
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateUnique<OpenGLRendererAPI>();
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Rudy