#pragma once
#include "RudyPCH.h"

#include "Runtime/Core/Window.h"
#include "Runtime/Components/Renderer/GraphicsContext.h"
#include "Runtime/Platform/OpenGL/OpenGLContext.h"

// new:
#include "Apps/Application.h"

// me:
//  WindowData:
//  group all the window related data into a struct similarly.
// including config,state, callback, etc.

namespace Rudy
{

class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps& props);
    ~WindowsWindow();

    //===inherited
    void OnUpdate() override;

    uint32_t GetWidth() const override { return m_Data.Width; }
    uint32_t GetHeight() const override { return m_Data.Height; }

    // Window attributes
    void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    bool ShouldClose() override;

    virtual void* GetNativeWindow() const { return m_Window; }

    //===========

    // child -specific
private:
    // utilities
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

private:
    GLFWwindow*                m_Window;
    UniquePtr<GraphicsContext> m_Context;

    struct WindowData
    {
        uint32_t    Width, Height;
        std::string Title = "Default";
        bool        VSync = true;

        // me: for robustness, when no callback is set, we set it to a dummy function that does nothing.
        EventCallbackFn EventCallback = [](Event& e)
        { RD_CORE_WARN("windowswindowEventCallBack: no callback of type:{0} is set", e.GetName()); };
    };

    WindowData m_Data;
};

} // namespace Rudy