#include "RudyPCH.h"

#include "Window.h"

#ifdef RD_PLATFORM_WINDOWS
#    include "Runtime/Platform/Windows/WindowsWindow.h"
#endif

namespace Rudy
{
SharedPtr<Window> Window::Create(const WindowProps& props)
{
#ifdef RD_PLATFORM_WINDOWS
    return CreateUnique<WindowsWindow>(props);
#else
    RD_CORE_ASSERT(false, "Unknown platform!");
    return nullptr;
#endif
}

} // namespace Rudy