
#include "RudyPCH.h"

#include "Runtime/Events/Eventhub.h"

#include "Runtime/Renderer/RendererAPI.h"

#include "GLFWWindow.h"

namespace Rudy
{

static uint8_t s_GLFWWindowCount = 0;

static void GLFWErrorCallback(int error, const char* description)
{
    RD_CORE_ERROR("WindowsWindow.cpp: GLFW Error ({0}): {1}", error, description);
}

UWindowsWindow::UWindowsWindow(const WindowCreateInfo& props)
{
    // RD_PROFILE_FUNCTION();
    Init(props);
}

UWindowsWindow::~UWindowsWindow()
{
    // RD_PROFILE_FUNCTION();
    Shutdown();
}

void UWindowsWindow::Init(const WindowCreateInfo& props)
{
    // RD_PROFILE_FUNCTION();

    windowData.Title  = props.Title;
    windowData.width  = props.Width;
    windowData.height = props.Height;

    RD_CORE_INFO("WindowsWindow: Creating window:{0} ({1}, {2})", props.Title, props.Width, props.Height);

    // if no window is created, initialize GLFW first;
    if (s_GLFWWindowCount == 0)
    {
        // RD_PROFILE_SCOPE("glfwInit first time");
        int success = glfwInit();
        RD_CORE_ASSERT(success, "Could not initialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
    }

    {
        // RD_PROFILE_SCOPE("glfwCreateWindow");
#if defined(_DEBUG)
        if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

        m_window = glfwCreateWindow((int)props.Width, (int)props.Height, windowData.Title.c_str(), nullptr, nullptr);
        ++s_GLFWWindowCount;
    }

    //====
    m_Context = GraphicsContext::Create(m_window);
    m_Context->Init();
    //====

    glfwSetWindowUserPointer(m_window, &windowData);

    SetVSync(m_VSync);

    // Set GLFW callbacks  using lambda expressions
    // bug history: runtime;  std::function,std::bad_function_call
    // need to set the "EventCallbackFn"  before calling it.
    // i find such coupling bad design, i just ignore it for now.

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
                              {
                                  WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                                  data.width       = width;
                                  data.height      = height;

                                  data.windowEventCallbackFn.BlockingBroadCast(WEWindowResize(width, height)); });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
                               {
                                   WindowData&      data = *(WindowData*)glfwGetWindowUserPointer(window);
                                   
                                   data.windowEventCallbackFn.BlockingBroadCast(WEWindowClose()); });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
                       {
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
                    data.inputEventCallbackFn.BlockingBroadCast(IEKeyPressed(key));  
					break;
				}
				case GLFW_RELEASE:
				{
                    data.inputEventCallbackFn.BlockingBroadCast(IEKeyReleased(key));
					break;
				}
				case GLFW_REPEAT:
				{
                    data.inputEventCallbackFn.BlockingBroadCast(IEKeyRepeat(key));
					break;
				}
				} });

    glfwSetCharCallback(m_window, [](GLFWwindow* window, uint32_t keycode)
                        {
                            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                            // KeyTypedEvent event(keycode);
                            //  data.EventCallback(event);
                        });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
                               {
		 	WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		 
		 	switch (action)
		 	{
		 	case GLFW_PRESS:
		 	{
              data.inputEventCallbackFn.BlockingBroadCast(IEMouseButtonPressed(button));
		 		break;
		 	}
		 	case GLFW_RELEASE:
		 	{
                  data.inputEventCallbackFn.BlockingBroadCast(IEMouseButtonReleased(button));
		 		//data.EventCallback(event);
		 		break;
		 	}
		 	} });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
                          {
                              WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                              data.inputEventCallbackFn.BlockingBroadCast(IEMouseSroll((float)xOffset, (float)yOffset));

                              // MouseScrolledEvent event((float)xOffset, (float)yOffset);
                              //  data.EventCallback(event);
                          });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
                             {
                                 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                                 data.inputEventCallbackFn.BlockingBroadCast(IEMouseCursorMoved((float)xPos, (float)yPos));

                                 // MouseMovedEvent event((float)xPos, (float)yPos);
                                 //  data.EventCallback(event);
                             });
}

void UWindowsWindow::Shutdown()
{
    // RD_PROFILE_FUNCTION();

    glfwDestroyWindow(m_window);
    --s_GLFWWindowCount;

    if (s_GLFWWindowCount == 0)
    {
        glfwTerminate();
    }
}

void UWindowsWindow::OnUpdate()
{
    // RD_PROFILE_FUNCTION();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void UWindowsWindow::SetVSync(bool enabled)
{
    // RD_PROFILE_FUNCTION();
    RD_CORE_INFO("WindowsWindow: VSync On: {0}", enabled);
    m_VSync = enabled;

    if (enabled)
        glfwSwapInterval(1); // lock to system refresh rate
    else
        glfwSwapInterval(0); // unlimited fps
}

bool UWindowsWindow::ShouldClose()
{
    return glfwWindowShouldClose(m_window);
}

} // namespace Rudy
