#pragma once
#include "EtherPCH.h"

#include "Hazel/Core/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>


//me:
// WindowData:
// group all the window related data into a struct similarly.
//including config,state, callback, etc.

 


namespace Hazel {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		//===me: inherited from Window
		void OnUpdate() override;

		uint32_t GetWidth() const override { return m_Data.Width; }
		uint32_t GetHeight() const override { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		bool ShouldClose() override;

		virtual void* GetNativeWindow() const { return m_Window; }

		//=====me: inherited from Window


	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title = "Default";
			uint32_t Width, Height;
			bool VSync;

			//me: for robustness, when no callback is set, we set it to a dummy function that does nothing.
			EventCallbackFn EventCallback = [](Event& e) {  HZ_CORE_WARN("windowswindowEventCallBack: no callback of type:{0} is set", e.GetName());  };
		};

		WindowData m_Data;
	};

}