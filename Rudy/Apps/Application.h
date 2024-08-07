#pragma once
#include "RudyPCH.h"
#include <Rudy.h>

namespace Rudy
{

//<<abstract>
class Application
{
public:
    virtual ~Application() = default;
    Application();

    virtual void Init();
    virtual void Run();
    virtual void ShutDown();

public:
    //<<singleton>>
    static Application* s_instance; // to access application;
    static Application* GetInstance();

    ViewportLayer* GetViewportLayer();

    // new: layer stack
    std::vector<Layer*> m_layers;
    ViewportLayer*      m_viewportLayer = nullptr;
    ImGuiLayer*         m_imguiLayer    = nullptr;
    void                PushLayer(Layer* layer);
    void                PopLayer(Layer* layer);
};

} // namespace Rudy
