#pragma once
#include "RudyPCH.h"

#include "Rudy/Core/Window.h"
#include "Rudy/Renderer/Camera.h"
#include "Rudy/Renderer/RendererAPI.h"

namespace Rudy
{

//<<interface>>
class Layer
{
public:
    virtual ~Layer() = default;
    Layer() {};

    // virtual void OnEvent(Event& event) { }
    virtual void Init() { }
    virtual void ShutDown() { }

    virtual void OnUpdate(float ts) { }
    virtual void OnImGuiRender() { } // customize for specific layer

    // const std::string& GetName() const { return m_DebugName; }
    // std::string m_DebugName;
};

// not optional for now;
// enum class RenderingEngine
//{

//    Renderer2D = 0,
//    Renderer3D = 1,
//    RayTracer  = 2,
//};
//

//<<base>>
class ViewportLayer : public Layer
{
public:
    virtual ~ViewportLayer() = default;
    ViewportLayer() :
        Layer() {};

    // inherted
    void Init() override;
    void ShutDown() override;

    // get/set
    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    static Ref<RendererAPI> GetRendererAPI() { return s_rendererAPI; }
    static void             SetRendererAPI(Ref<RendererAPI> api) { s_rendererAPI = api; }

    Ref<Window> GetWindow() { return m_window; }
    void        SetWindow(Ref<Window> window) { m_window = window; }
    Ref<Camera> GetMainCamera() { return m_mainCamera; }
    void        SetMainCamera(Ref<Camera> camera) { m_mainCamera = camera; }

    // new:  width/height
    float GetAspectRatio()
    {
        return m_window->GetWidth() / (float)m_window->GetHeight();
    }

    bool ShouldClose()
    {
        return m_window->ShouldClose();
    }

    void WindowOnUpdate()
    {
        m_window->OnUpdate();
    }

    void CameraOnUpdate(float ts)
    {
        m_mainCamera->OnUpdate(ts);
    }

    void OnWindowResize(uint32_t width, uint32_t height);

    // static void Render(const Ref<Scene>& scene);
    //  static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

protected:
    static Ref<RendererAPI> s_rendererAPI;
    Ref<Camera>             m_mainCamera = nullptr;
    Ref<Window>             m_window     = nullptr;

public:
    // default values
    std::string m_windowTitle = "Rudy Engine Default";

    glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 1.5f, 5.0f);

    // 2560:1440 = 16:9
    uint32_t SCR_WIDTH  = 2560;
    uint32_t SCR_HEIGHT = 1440;
};

class ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer() = default;

    // virtual void OnEvent(Event& e) override;

    virtual void Init() override;
    virtual void ShutDown() override;

    // update in stages:  gui->begin; foreach layer->GuiRender ; gui->end
    void BeginUpdate();
    void EndUpdate();

    // optionally to block events
    void BlockEvents(bool block) { m_BlockEvents = block; }

private:
    bool m_BlockEvents = true;
};

} // namespace Rudy