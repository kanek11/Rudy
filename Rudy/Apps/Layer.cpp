#include "RudyPCH.h"
#include "Layer.h"

#include "Application.h"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw.h"
#include "ThirdParty/imgui/imgui_impl_opengl3.h"

namespace Rudy
{
SharedPtr<RendererAPI> ViewportLayer::s_rendererAPI = nullptr;

ImGuiLayer::ImGuiLayer()
{
    this->m_BlockEvents = true;
}

void ImGuiLayer::Init()
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
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)_viewport->GetWindow()->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // other settings
    ImGui::SetNextWindowSize(ImVec2(500, 400)); // 设置窗口大小为 500x400

    // initial position on the right top corner;
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(_viewport->SCR_WIDTH - 500), 0)); // 设置窗口位置为 (SCR_WIDTH - 500, 0)

    ImGui::CreateContext();
    ImGuiIO&     io = ImGui::GetIO();
    ImFontConfig fontConfig;
    fontConfig.SizePixels = 18.0f;
    io.Fonts->AddFontDefault(&fontConfig);
}

void ImGuiLayer::ShutDown()
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

//================================================================================================
void ViewportLayer::Init()
{
    RD_CORE_WARN("ViewportLayer: Init");

    // fixed dependency:
    // set API;  create API;  attach to renderer window
    RendererAPI::SetAPI(RendererAPI::API::OpenGL);
    auto _rendererAPI = RendererAPI::Create();
    ViewportLayer::SetRendererAPI(_rendererAPI);

    // window  fix: the context binding is unclear
    auto _window = Window::Create(WindowProps { SCR_WIDTH, SCR_HEIGHT, m_windowTitle });
    ViewportLayer::SetWindow(_window); // fix , this is not static:
    Input::SetWindowContext(_window->GetNativeWindow());
    _window->SetVSync(false);
}

void ViewportLayer::ShutDown()
{
    RD_CORE_WARN("ViewportLayer: shutdown");
}

void ViewportLayer::OnWindowResize(uint32_t width, uint32_t height)
{
    // RenderCommand::SetViewport(0, 0, width, height);
}

// void ViewportLayer::Render(const SharedPtr<Scene>& scene)
//{
//  RD_PROFILE_FUNCTION();

// s_SceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();

// loop objects in the scene
// auto group = scene->m_Registry.group<TransformComponent>(entt::get<MeshComponent>);

// for (auto obj : scene->Objects)
//{
//	//todo:  only for renderable objects
//	//obj->Draw();
//
// }

// auto view = scene->m_Registry.view<TransformComponent, MeshComponent>();
// for (auto entity : view)
//{
//	auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

//	Renderer3D::Submit(mesh.mesh, transform.GetTransform());
//}
//}

// void Renderer::Submit(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertexArray, const glm::mat4& transform)
//{
//	shader->Bind();
//	shader->SetMat4("u_viewProjection", s_SceneData->ViewProjectionMatrix);
//	shader->SetMat4("u_Transform", transform);

//	vertexArray->Bind();
//	RenderCommand::DrawIndexed(vertexArray);
//}

} // namespace Rudy