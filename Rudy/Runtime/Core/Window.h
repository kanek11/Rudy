#pragma once
#include "RudyPCH.h"

#include "Runtime/Events/Delegate.h"
#include "Runtime/Events/Eventhub.h"

// user-defined Event callback function, like a delegate in C#.
// a void function that takes an Event& as a parameter.

// get the native window handle, for example, in Windows, it's HWND.

namespace Rudy
{

struct WindowCreateInfo
{
    uint32_t    Width;
    uint32_t    Height;
    std::string Title;

    WindowCreateInfo(uint32_t           width  = 1600,
                     uint32_t           height = 900,
                     const std::string& title  = "Rudy Engine") :
        Width(width),
        Height(height), Title(title)
    {
    }
};

//<<interface>>
class Window
{
public:
    // update
    virtual void OnUpdate()    = 0;
    virtual bool ShouldClose() = 0;

    // query
    virtual uint32_t GetWidth() const  = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual void* GetNativeWindow() const = 0;

    // Window system attributes
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const        = 0;

    //
    static SharedPtr<Window> CreateAndSetGLContext(const WindowCreateInfo& props = WindowCreateInfo());

public:
    struct WindowData
    {
        uint32_t    width, height;
        std::string Title;

        FDelegate<void(WindowEvent)> windowEventCallbackFn;
        FDelegate<void(InputEvent)>  inputEventCallbackFn;
    };

    WindowData windowData;
    bool       m_VSync;
};

} // namespace Rudy
