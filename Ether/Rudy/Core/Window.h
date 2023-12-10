#pragma once

//  Core/

#include "RudyPCH.h"
 
#include "Rudy/Events/Event.h"


//me:
//it's good practice to group related data into a structured data type.
//design principle:  
//Single Responsibility Principle ,  all data is grouped into a single data type. 
//instead of having a bunch of variables floating around in the Window class.



//interface doc: 
//window property is common to all window systems.
//Name, Width, Height 

//user-defined Event callback function, like a delegate in C#.
//a void function that takes an Event& as a parameter.

//get the native window handle, for example, in Windows, it's HWND.

//static function to create a window,
//interface is not intended to be instantiated per se, it's a contract for implementing classes.
//we use Window::Create() to create a window, regardless of the implementation.




namespace Rudy {

	struct WindowProps
	{
		uint32_t Width;
		uint32_t Height; 
		std::string Title;

		WindowProps(uint32_t width = 1600,
			        uint32_t height = 900,
			        const std::string& title = "Rudy Engine")
			:  Width(width), Height(height), Title(title)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual bool ShouldClose() = 0;

		virtual void* GetNativeWindow() const = 0;

		//static
		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}


