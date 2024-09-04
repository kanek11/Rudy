// ver 2024.1.26
#include "RudyPCH.h"

#include "Rudy.h"

#include "Runtime/Renderer/CubemapPasses/RecToCubePass.h"

using namespace Rudy;
using namespace std;

class TestComponent : public UActorComponent
{
public:
    virtual void TickComponent(float DeltaTime) override
    {
        UActorComponent::TickComponent(DeltaTime);
        std::cout << "TickComponent: " << DeltaTime << std::endl;
    }
};

class TestActor : public AActor
{
public:
    TestActor()
    {
        this->testSubObject = CreateComponentAsSubObject<TestComponent>();
    }

    virtual void Tick(float DeltaTime) override
    {
        AActor::Tick(DeltaTime);
        std::cout << "TickActor" << std::endl;
    }

    virtual void BeginPlay() override
    {
        std::cout << "testActor::BeginPlay" << std::endl;
    }

    virtual void EndPlay() override
    {
        std::cout << "testActor::EndPlay" << std::endl;
    }

    SharedPtr<TestComponent> testSubObject = nullptr;
};

int main()
{
    // unit test kinda thing
    Rudy::Log::Init();

    {
        auto                  src_path  = std::source_location::current();
        std::filesystem::path this_file = src_path.file_name(); // neccessary conversion

        std::filesystem::current_path(this_file.parent_path().parent_path());
        RD_CORE_INFO("Set Current working directory: {0}", std::filesystem::current_path().string());
    }

    RendererAPI::SetAPI(RendererAPI::API::OpenGL);
    auto _rendererAPI = RendererAPI::Create();
    ViewportLayer::SetRendererAPI(_rendererAPI);

    // window
    int  width = 1280, height = 720;
    auto _window = Window::Create(WindowProps { 1280, 720, "Test" });
    Input::SetWindowContext(_window->GetNativeWindow());

    auto main_camera  = CreateActor<ACameraActor>();
    main_camera->Name = "Main Camera";

    // Actor
    if (true)
    {
        auto testActor = CreateActor<TestActor>();

        auto actorComponent = CreateActorComponent<TestComponent>();

        // dynamic
        testActor->AddComponent(actorComponent);
        actorComponent->RegisterOwner(testActor);

        // tick one
        auto _component = testActor->GetComponent<UActorComponent>();
        _component->TickComponent(0.1f);

        // tick both
        testActor->Tick(0.1f);
    }

    //  scene component
    if (true)
    {
        auto sceneComp  = CreateActorComponent<USceneComponent>();
        auto sceneComp2 = CreateActorComponent<USceneComponent>();

        sceneComp2->AttachToParent(sceneComp);
        std::cout << sceneComp->GetChildrenCount() << std::endl;
        RD_CORE_ASSERT(sceneComp2->GetParent() != nullptr, "Parent is empty");

        sceneComp->SetLocalPosition(glm::vec3(1.0f, 1.0f, 1.0f));
        sceneComp2->SetLocalPosition(glm::vec3(1.0f, 1.0f, 1.0f));

        sceneComp->RecalcWorldTransformHierarchy();
        std::cout << sceneComp2->GetWorldPosition() << std::endl;

        sceneComp2->SetWorldPosition(glm::vec3(5.0f, 5.0f, 5.0f));

        sceneComp2->RecalcWorldTransformHierarchy();
        std::cout << sceneComp2->GetWorldPosition() << std::endl;
    }

    {
        TextureImporter hdrImporter;
        hdrImporter.bFlipYOnLoad = true;
        hdrImporter.bIsHDR       = true;
        auto origin_env2D        = hdrImporter.ImportTexture2D("Resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr");

        //
        auto recToCubePass     = CreateShared<CubemapPrecomputePass>();
        auto origin_envCubemap = recToCubePass->RecToCube(origin_env2D);
        auto spec_envCubemap   = recToCubePass->SpecularPrefilter(origin_envCubemap);

        auto skyboxShader = Shader::Create("skybox shader",
                                           "Shaders/Shaders/Skybox_VS.glsl",
                                           "Shaders/Shaders/Skybox_FS.glsl");

        auto skyboxMaterial = Material::Create("skyboxMaterial");
        skyboxMaterial->SetTexture(TexType::SkyboxTexture, spec_envCubemap);

        auto skybox  = CreateActor<Cube>();
        skybox->Name = "skybox";
        skybox->SetShader(skyboxShader);
        skybox->SetMaterial(skyboxMaterial);

        auto screenQuadShader   = Shader::Create("screen quad shader",
                                               "Shaders/Shaders/ScreenQuad_VS.glsl",
                                               "Shaders/Shaders/ScreenQuad_FS.glsl");
        auto screenQuadMaterial = Material::Create("screenQuadMaterial");

        auto screenQuad  = CreateActor<ScreenQuad>();
        screenQuad->Name = "screenQuad";
        screenQuad->SetShader(screenQuadShader);
        screenQuad->SetMaterial(screenQuadMaterial);

        float lastFrameTime = 0.0f;

        // game loop
        while (true)
        {
            float time      = (float)glfwGetTime();
            float deltaTime = time - lastFrameTime;
            lastFrameTime   = time;

            main_camera->Tick(deltaTime);
            skybox->Tick(deltaTime);

            if (false)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glClearColor(1.0f, 0.1f, 0.5f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glViewport(0, 0, width, height);
                glDisable(GL_DEPTH_TEST);

                screenQuadShader->Bind();

                glBindTextureUnit(0, origin_env2D->GetGPUID());
                screenQuad->DrawSelfContained();

                screenQuadShader->Unbind();
            }

            if (true)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glClearColor(1.0f, 0.1f, 0.5f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glViewport(0, 0, width, height);
                glDisable(GL_DEPTH_TEST);

                auto _skyboxShader = skybox->StaticMeshComponent->GetShader();
                _skyboxShader->Bind();

                auto _view = main_camera->CameraComponent->GetViewMatrix();
                // remove the translation part of the view matrix
                _view = glm::mat4(glm::mat3(_view));

                _skyboxShader->SetMat4("u_view", _view);
                _skyboxShader->SetMat4("u_projection", main_camera->CameraComponent->GetProjectionMatrix());

                _skyboxShader->SetFloat("u_mipLevel", 1.0f);

                skybox->StaticMeshComponent->SetGeometryState(_skyboxShader);
                skybox->StaticMeshComponent->SetMaterialState(_skyboxShader);

                skybox->DrawSkybox();

                _skyboxShader->Unbind();
            }

            _window->OnUpdate();
        }
    }

    glfwTerminate();
}
