#include "RudyPCH.h"

#include "Window.h"


#ifdef RD_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Rudy
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef RD_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		RD_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

}