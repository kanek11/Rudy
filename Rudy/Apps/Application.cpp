#include "RudyPCH.h"
#include "Application.h"

namespace Rudy
{

Application* Application::s_instance = nullptr;

Application::Application()
{
    RD_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;
}

Application* Application::GetInstance() { return s_instance; }

ViewportLayer* Application::GetViewportLayer() { return m_viewportLayer; }

void Application::PushLayer(Layer* layer)
{
    this->m_layers.push_back(layer);
}

void Application::PopLayer(Layer* layer)
{
    auto it = std::find(this->m_layers.begin(), this->m_layers.end(), layer);
    if (it != this->m_layers.end())
    {
        this->m_layers.erase(it);
    }
}

void Application::Init()
{
    //========================================
    m_viewportLayer->Init();

    for (Layer* layer : this->m_layers)
    {
        layer->Init();
    }

    // after opengl and glfw is initialized
    m_imguiLayer->Init();
}

//======the loop
/* Loop until the user closes the window */

// float lastFrameTime   = 0.0f;
// float timer = 0.0f;
// RD_CORE_WARN("App: Entering the loop");
// while (!RendererApp::ShouldClose())
//{
//     // RD_PROFILE_SCOPE("the game loop");

//    // get the time of each frame
//    float time      = (float)glfwGetTime();
//    float deltaTime = time - lastFrameTime;
//    lastFrameTime   = time;
//    timer += deltaTime;

//'gui'
// this->PrepareGUI();

void Application::Run()
{
    float lastFrameTime = 0.0f;
    RD_CORE_WARN("App: the game loop");
    while (!m_viewportLayer->ShouldClose())
    {
        // RD_PROFILE_SCOPE("the game loop");

        // get the time of each frame
        float time      = (float)glfwGetTime();
        float deltaTime = time - lastFrameTime;
        lastFrameTime   = time;

        for (Layer* layer : this->m_layers)
        {
            layer->OnUpdate(deltaTime);
        }

        m_viewportLayer->OnUpdate(deltaTime);

        this->m_imguiLayer->BeginUpdate();
        for (Layer* layer : this->m_layers)
        {
            layer->OnImGuiRender();
        }
        m_viewportLayer->OnImGuiRender();
        this->m_imguiLayer->EndUpdate();

        m_viewportLayer->WindowOnUpdate();
    }
}

void Application::ShutDown()
{
    for (Layer* layer : this->m_layers)
    {
        layer->ShutDown();
    }
    m_imguiLayer->ShutDown();

    // RendererApp::Shutdown();
    glfwTerminate();
}

} // namespace Rudy