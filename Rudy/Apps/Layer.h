#pragma once
#include "RudyPCH.h"

#include "Runtime/Core/Window.h"
#include "Runtime/Renderer/RendererAPI.h"

namespace Rudy
{

//<<interface>>
class Layer
{
public:
    virtual ~Layer() = default;

    // virtual void OnEvent(Event& event) { }
    virtual void OnInit() { }
    virtual void OnShutDown() { }

    virtual void OnUpdate(float ts) { }
    virtual void OnImGuiRender() { } // customize for specific layer

    // new:
    virtual void OnWindowEvent(const WindowEvent& event) {};
    virtual void OnInputEvent(const InputEvent& event) { }
};

//<<base>>
class ViewportLayer : public Layer
{
public:
    virtual ~ViewportLayer() = default;
    ViewportLayer(uint32_t width = 2560, uint32_t height = 1440) :
        SCR_WIDTH(width), SCR_HEIGHT(height) { }

    // inherted
    void OnInit() override;
    void OnShutDown() override;

    // get/set
    static RendererAPI::API       GetAPI() { return RendererAPI::GetAPI(); }
    static SharedPtr<RendererAPI> GetRendererAPI() { return s_rendererAPI; }
    static void                   SetRendererAPI(SharedPtr<RendererAPI> api) { s_rendererAPI = api; }

    SharedPtr<Window> GetWindow() { return m_window; }
    void              SetWindow(SharedPtr<Window> window) { m_window = window; }

    bool ShouldClose()
    {
        return m_window->ShouldClose();
    }

    void WindowOnUpdate()
    {
        m_window->OnUpdate();
    }

    virtual void OnWindowEvent(const WindowEvent& event) override
    {
        std::visit([this](auto&& e)
                   {
			using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, WEWindowResize>)
            {
                this->OnWindowResize(e.width, e.height);
			} },
                   event);
    }

    void OnWindowResize(uint32_t width, uint32_t height);

    // static void Render(const SharedPtr<Scene>& scene);
    //  static void Submit(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

protected:
    static SharedPtr<RendererAPI> s_rendererAPI;
    SharedPtr<Window>             m_window = nullptr;

public:
    // default values
    std::string m_windowTitle = "Rudy Engine Default";

    // 2560:1440 = 16:9
    uint32_t SCR_WIDTH  = 2560;
    uint32_t SCR_HEIGHT = 1440;
};

class ImGuiLayer : public Layer
{
public:
    ~ImGuiLayer() = default;

    // virtual void OnEvent(Event& e) override;

    virtual void OnInit() override;
    virtual void OnShutDown() override;

    virtual void OnImGuiRender() override;

    // update in stages:  gui->begin; foreach layer->GuiRender ; gui->end
    void BeginUpdate();
    void EndUpdate();
};

} // namespace Rudy