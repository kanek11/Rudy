#include "RudyPCH.h"

#include "Window.h"

#ifdef RD_PLATFORM_WINDOWS
#    include "Runtime/Platform/Windows/GLFWWindow.h"
#endif

namespace Rudy
{
SharedPtr<Window> Window::CreateAndSetGLContext(const WindowCreateInfo& props)
{
#ifdef RD_PLATFORM_WINDOWS
    return CreateUnique<UWindowsWindow>(props);
#else
    RD_CORE_ASSERT(false, "Unknown platform!");
    return nullptr;
#endif
}

} // namespace Rudy