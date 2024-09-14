#include "RudyPCH.h"
#include "Layer.h"

#include "Application.h"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw.h"
#include "ThirdParty/imgui/imgui_impl_opengl3.h"

namespace Rudy
{
SharedPtr<RendererAPI> ViewportLayer::s_rendererAPI = nullptr;

void ImGuiLayer::OnInit()
{
    RD_CORE_WARN("imguiLayer: Init");

    auto _app      = Application::GetInstance();
    auto _viewport = _app->m_viewportLayer;

    // initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // style
    ImGui::StyleColorsDark();

    // backends
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)(_viewport->GetWindow()->GetNativeWindow()), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // other settings
    ImGui::SetNextWindowSize(ImVec2(500, 400));

    // initial position on the right top corner;
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(_viewport->SCR_WIDTH - 500), 0));

    ImGui::CreateContext();
    ImGuiIO&     io = ImGui::GetIO();
    ImFontConfig fontConfig;
    fontConfig.SizePixels = 18.0f;
    io.Fonts->AddFontDefault(&fontConfig);
}

void ImGuiLayer::OnShutDown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::BeginUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // this->DrawGUI(); // draw the GUI for each layer
}

void ImGuiLayer::EndUpdate()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::OnImGuiRender()
{
    ImGuiIO& io = ImGui::GetIO();
    // dock to right top corner
    float width = 500, height = 400;
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(Application::GetInstance()->GetViewportLayer()->SCR_WIDTH - 500), 0));
    ImGui::SetNextWindowSize(ImVec2(width, io.DisplaySize.y));
}

//================================================================================================
void ViewportLayer::OnInit()
{
    RD_CORE_WARN("ViewportLayer: Init");

    // fixed dependency:
    // set API;  create API;  attach to renderer window
    RendererAPI::SetAPI(RendererAPI::API::OpenGL);
    auto _rendererAPI = RendererAPI::Create();
    ViewportLayer::SetRendererAPI(_rendererAPI);

    // window  fix: the context binding is unclear
    auto _window = Window::CreateAndSetGLContext(WindowCreateInfo { SCR_WIDTH, SCR_HEIGHT, m_windowTitle });
    ViewportLayer::SetWindow(_window); // fix , this is not static:
    InputManager::SetWindowContext(_window->GetNativeWindow());
    _window->SetVSync(false);

    // new:
    _window->windowData.windowEventCallbackFn.Add(BIND_MEM_FN(ViewportLayer::OnWindowEvent));
}

void ViewportLayer::OnShutDown()
{
    RD_CORE_WARN("ViewportLayer: shutdown");
}

void ViewportLayer::OnWindowResize(uint32_t width, uint32_t height)
{
    SCR_HEIGHT = height;
    SCR_WIDTH  = width;

    RD_CORE_INFO("ViewportLayer: OnWindowResize: {0}, {1}", width, height);
}
} // namespace Rudy