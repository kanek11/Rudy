#pragma once
#include "RudyPCH.h"

#include "Runtime/Core/Window.h"
#include "Runtime/Platform/OpenGL/OpenGLContext.h"

// me:
//  WindowData:
//  group all the window related data into a struct similarly.
// including config,state, callback, etc.

namespace Rudy
{

class UWindowsWindow : public Window
{
public:
    UWindowsWindow(const WindowCreateInfo& props);
    ~UWindowsWindow();

    //===inherited
    void OnUpdate() override;

    uint32_t GetWidth() const override { return windowData.width; }
    uint32_t GetHeight() const override { return windowData.height; }

    // Window attributes
    void SetVSync(bool enabled) override;
    bool IsVSync() const override { return m_VSync; }

    bool ShouldClose() override;

    virtual void* GetNativeWindow() const { return static_cast<void*>(m_window); }

    //===========

    // child -specific
private:
    // utilities
    virtual void Init(const WindowCreateInfo& props);
    virtual void Shutdown();

private:
    GLFWwindow*                m_window;
    UniquePtr<GraphicsContext> m_Context;
};

} // namespace Rudy