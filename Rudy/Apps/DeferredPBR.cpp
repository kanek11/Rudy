
#include "RudyPCH.h"

#include "deferredPBR.h"

#include <glm/glm.hpp>

namespace Rudy
{

DeferredPBR::DeferredPBR() :
    ViewportLayer()
{
}

void DeferredPBR::ShutDown()
{
    // all lifetime management is done by the smart pointers;
}

void DeferredPBR::Init()
{
    // impose: init base class;
    ViewportLayer::Init();

    //=============================
    //=== initialize resources

    // camera
    main_camera       = CreateActor<ACameraActor>();
    main_camera->Name = "Main Camera";

    // lighting
    sunlight       = CreateActor<ASunlightActor>();
    sunlight->Name = "Sunlight";
    sunlight->SunlightComponent->SetIntensity(5.0f);
    sunlight->SunlightComponent->SetDirection(glm::vec3(1.0f, -2.0f, -1.0f));

    //=======environment map precomputing

    auto precomputeCube = CreateShared<PrecomputeCubemapPass>();

    TextureImporter hdrImporter;
    hdrImporter.bFlipYOnLoad = true;
    hdrImporter.bIsHDR       = true;
    debugHDRI                = hdrImporter.ImportTexture2D("Resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr");

    auto origin_env2D  = hdrImporter.ImportTexture2D("Resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr");
    auto origin_envMap = precomputeCube->RecToCube(origin_env2D);

    // ifprecomputed envmap available
    auto diffuse_env2D  = hdrImporter.ImportTexture2D("Resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_Env.hdr");
    auto diffuse_envMap = precomputeCube->RecToCube(diffuse_env2D);

    // lod 5 for specular envmap;
    auto specular_envMap = precomputeCube->SpecularPrefilter(origin_envMap);

    // BRDF integration map
    auto brdfLUTShader    = Shader::Create("brdfLUT Shader",
                                        "Shaders/Shaders/BrdfLUT_VS.glsl",
                                        "Shaders/Shaders/BrdfLUT_FS.glsl");
    auto brdfQuadMaterial = Material::Create("brdfquad material");

    // don't forget to RGBA if ever use as image2D
    brdfLUTTexture = NewObject<UTexture2D>("brdfLUTTexture",
                                           TextureConfig { 512, 512, TexFormat::RGB32F });

    auto brdfLUTFBO = FrameBuffer::Create("brdfLUT FBO", 512, 512, FrameBufferType::Regular);

    brdfLUTFBO->SetColorTexture(TexType::brdfLUTTexture, brdfLUTTexture, 0);

    brdfLUTFBO->Bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, 512, 512);

    auto brdfQuad  = CreateActor<ScreenQuad>();
    brdfQuad->Name = "brdfQuad";
    brdfQuad->StaticMeshComponent->SetShader(brdfLUTShader);
    brdfQuad->StaticMeshComponent->SetMaterial(brdfQuadMaterial);

    brdfQuad->DrawSelfContained();

    brdfLUTFBO->Unbind();

    //==========shadow map pass
    shadowMapShader = Shader::Create("shadowMap Shader",
                                     "Shaders/Shaders/DepthMap_VS.glsl",
                                     "Shaders/Shaders/DepthMap_FS.glsl");

    auto shadowMapSkinnedShader = Shader::Create("shadowMap skinned Shader",
                                                 "Shaders/Shaders/DepthMapSkinned_VS.glsl",
                                                 "Shaders/Shaders/DepthMap_FS.glsl");

    shadowMapMaterial        = Material::Create("shadowMapMaterial");
    shadowMapSkinnedMaterial = Material::Create("shadowMapSkinnedMaterial");

    shadowMapFBO = FrameBuffer::Create("shadowMap FBO",
                                       SHADOW_WIDTH,
                                       SHADOW_HEIGHT,
                                       FrameBufferType::DepthTexture);

    shadowMap = NewObject<UTexture2D>("shadowMap",
                                      TextureConfig {
                                          SHADOW_WIDTH, SHADOW_HEIGHT, TexFormat::DEPTH_COMPONENT24, 1, false, WrapMode::CLAMP_TO_BORDER, FilterMode::NEAREST });

    // set the border color for the depth texture;

    float borderColor2[] = { 1.0, 1.0, 1.0, 1.0 };
    glTextureParameterfv(shadowMap->GetID(), GL_TEXTURE_BORDER_COLOR, borderColor2);

    shadowMapFBO->SetDepthTexture(shadowMap);
    // shadowMapFBO->FinishSetup();

    //======gbuffer pass;
    // the framebuffer for gbuffer pass: 8;
    auto gWorldPosition = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::RGBA32F });
    auto gWorldNormal   = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::RGBA32F });
    auto gWorldTangent  = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::RGBA32F });

    auto gAlbedo    = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::RGBA32F });
    auto gSpecular  = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::R32F });
    auto gMetallic  = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::R32F });
    auto gRoughness = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::R32F });
    auto gViewDepth = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::DEPTH_COMPONENT24, 1, false, WrapMode::CLAMP_TO_BORDER, FilterMode::NEAREST });

    // new: extension of gbuffers;
    auto gViewPosition = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::RGBA32F });
    auto gViewNormal   = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::RGBA32F });

    {
        gBufferPassShader = Shader::Create("gBuffer Shader",
                                           "Shaders/Deferred/GBuffer_VS.glsl",
                                           "Shaders/Deferred/GBuffer_FS.glsl");

        // Framebuffer;
        gBufferFBO = FrameBuffer::Create(
            "GBuffer FBO", SCR_WIDTH, SCR_WIDTH, FrameBufferType::GBuffer);

        // set the border color for the depth texture;
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTextureParameterfv(gViewDepth->GetID(), GL_TEXTURE_BORDER_COLOR, borderColor);

        gBufferFBO->SetColorTexture(TexType::gWorldPosition, gWorldPosition, 0);
        gBufferFBO->SetColorTexture(TexType::gAlbedo, gAlbedo, 1);
        gBufferFBO->SetColorTexture(TexType::gWorldNormal, gWorldNormal, 2);
        gBufferFBO->SetColorTexture(TexType::gWorldTangent, gWorldTangent, 3);
        gBufferFBO->SetColorTexture(TexType::gSpecular, gSpecular, 4);
        gBufferFBO->SetColorTexture(TexType::gMetallic, gMetallic, 5);
        gBufferFBO->SetColorTexture(TexType::gRoughness, gRoughness, 6);

        gBufferFBO->SetDepthTexture(gViewDepth);

        gBufferFBO->FinishSetup();
    }

    //===============================
    //======lit pass

    // auto litPassShader = Shader::Create("blinnPhong Shader", "Shaders/Shaders/BlinnPhong_VS.glsl", "Shaders/Shaders/BlinnPhong_FS.glsl");
    this->litPassShader = Shader::Create("pbr Shader",
                                         "Shaders/Deferred/PBR_VS.glsl",
                                         "Shaders/Deferred/PBR_FS.glsl");

    //=== FBO
    this->litPassFBO = FrameBuffer::Create("litPass FBO",
                                           SCR_WIDTH,
                                           SCR_WIDTH,
                                           FrameBufferType::Regular);

    auto litPassScreenTexture = NewObject<UTexture2D>("", TextureConfig { SCR_WIDTH, SCR_HEIGHT, TexFormat::RGBA32F, 1, false, WrapMode::CLAMP_TO_BORDER, FilterMode::LINEAR, FilterMode::LINEAR });

    litPassFBO->SetColorTexture(TexType::ScreenTexture, litPassScreenTexture, 0);
    litPassFBO->FinishSetup();

    litOutputs[TexType::ScreenTexture] = litPassScreenTexture;

    //=== material

    auto litPassMaterial = Material::Create("litPassMaterial");

    litPassMaterial->SetTexture(TexType::gWorldPosition, gWorldPosition);
    litPassMaterial->SetTexture(TexType::gWorldNormal, gWorldNormal);
    litPassMaterial->SetTexture(TexType::gWorldTangent, gWorldTangent);
    litPassMaterial->SetTexture(TexType::gAlbedo, gAlbedo);
    litPassMaterial->SetTexture(TexType::gSpecular, gSpecular);
    litPassMaterial->SetTexture(TexType::gMetallic, gMetallic);
    litPassMaterial->SetTexture(TexType::gRoughness, gRoughness);

    litPassMaterial->SetTexture(TexType::gViewDepth, gViewDepth);

    litPassMaterial->SetTexture(TexType::diffuseEnvMap, diffuse_envMap);
    litPassMaterial->SetTexture(TexType::specularEnvMap, specular_envMap);
    litPassMaterial->SetTexture(TexType::brdfLUTTexture, brdfLUTTexture);

    litPassMaterial->SetTexture(TexType::DepthTexture, shadowMap);

    // render quad;
    litPassQuad       = CreateActor<ScreenQuad>();
    litPassQuad->Name = "litPassQuad";
    litPassQuad->StaticMeshComponent->SetShader(litPassShader);
    litPassQuad->StaticMeshComponent->SetMaterial(litPassMaterial);

    //=======actors

    if (true)
    {
        auto floor_gMaterial = PBRMaterial::Create("gBufferPassMaterial");

        TextureImporter pngImporter;

        auto floor_albedoMap    = pngImporter.ImportTexture2D("Resources/PBRTextures/Floor_brown_ue/albedo.png");
        auto floor_normalMap    = pngImporter.ImportTexture2D("Resources/PBRTextures/Floor_brown_ue/normal.png");
        auto floor_roughnessMap = pngImporter.ImportTexture2D("Resources/PBRTextures/Floor_brown_ue/roughness.png");
        auto floor_metallicMap  = pngImporter.ImportTexture2D("Resources/PBRTextures/Floor_brown_ue/metallic.png");

        floor_gMaterial->SetTexture(TexType::AlbedoMap, floor_albedoMap);
        floor_gMaterial->SetTexture(TexType::NormalMap, floor_normalMap);
        floor_gMaterial->SetTexture(TexType::RoughnessMap, floor_roughnessMap);
        floor_gMaterial->SetTexture(TexType::MetallicMap, floor_metallicMap);

        auto floor1  = CreateActor<Plane>(10);
        floor1->Name = "floor1";

        floor1->SetShader(gBufferPassShader);
        floor1->SetMaterial(floor_gMaterial);
        floor1->RootComponent->SetLocalScale(glm::vec3(20.0f));
        staticMeshActors.push_back(floor1);
    }
    // color of gold
    glm::vec3 gold_color = glm::vec3(1.0f, 0.766f, 0.336f);

    // draw five spheres each with different roughness
    if (true)
    {
        for (int i = 0; i < 6; i++)
        {
            auto sphere_gMaterial = PBRMaterial::Create("gBufferPassMaterial");

            sphere_gMaterial->SetVec3("u_Albedo", gold_color);
            sphere_gMaterial->SetFloat("u_Metallic", 1.0f);
            sphere_gMaterial->SetFloat("u_Roughness", (float)i / 5.0f);

            auto sphere  = CreateActor<Sphere>(20);
            sphere->Name = "sphere" + std::to_string(i);

            sphere->SetShader(gBufferPassShader);
            sphere->SetMaterial(sphere_gMaterial);
            sphere->RootComponent->SetLocalPosition(glm::vec3(-3.0f + i * 2.0f, +1.0f, 0.0f));

            this->staticMeshActors.push_back(sphere);
        }
    }

    // a cube
    {
        // silver color
        glm::vec3 silver = glm::vec3(0.972f, 0.960f, 0.915f);

        auto cube_gMaterial = PBRMaterial::Create("gBufferPassMaterial");

        cube_gMaterial->SetVec3("u_Albedo", silver);
        cube_gMaterial->SetFloat("u_Metallic", 1.0f);
        cube_gMaterial->SetFloat("u_Roughness", 0.0f);

        auto cube  = CreateActor<Cube>();
        cube->Name = "cube0";

        cube->SetShader(gBufferPassShader);
        cube->SetMaterial(cube_gMaterial);
        cube->RootComponent->SetLocalPosition(glm::vec3(0.0f, 1.0f, -3.0f));

        staticMeshActors.push_back(cube);
    }

    // world to view
    {
        WorldToViewInputs[TexType::gWorldPosition] = gWorldPosition;
        WorldToViewInputs[TexType::gWorldNormal]   = gWorldNormal;
        WorldToViewInputs[TexType::gViewDepth]     = gViewDepth;

        WorldToViewOutputs[TexType::gViewPosition] = gViewPosition;
        WorldToViewOutputs[TexType::gViewNormal]   = gViewNormal;

        WorldToViewPass = CreateShared<WorldToView>(SCR_WIDTH, SCR_HEIGHT, WorldToViewInputs, WorldToViewOutputs);
    }

    // SSAO
    {
        SSAOInputs[TexType::gViewPosition] = gViewPosition;
        SSAOInputs[TexType::gViewNormal]   = gViewNormal;
        // SSAOInputs[TexType::gWorldTangent] = gWorldTangent;
        SSAOInputs[TexType::gViewDepth] = gViewDepth;

        SSAOPass = CreateShared<SSAO>(SCR_WIDTH, SCR_HEIGHT, SSAOInputs, SSAOOutputs);
    }

    // Bloom
    {
        BloomInputs[TexType::ScreenTexture] = litPassScreenTexture;

        BloomPass = CreateShared<Bloom>(SCR_WIDTH, SCR_HEIGHT, BloomInputs, BloomOutputs);
    }

    // Outline
    {
        OutlineInputs[TexType::gViewPosition] = gViewPosition;
        OutlineInputs[TexType::gViewNormal]   = gViewNormal;

        OutlinePass = CreateShared<Outline>(SCR_WIDTH, SCR_HEIGHT, OutlineInputs, OutlineOutputs);
    }

    // SSR
    {
        SSRInputs[TexType::gViewPosition] = gViewPosition;
        SSRInputs[TexType::gViewNormal]   = gViewNormal;
        SSRInputs[TexType::ScreenTexture] = litPassScreenTexture;
        SSRInputs[TexType::gViewDepth]    = gViewDepth;

        SSRInputs[TexType::gRoughness] = gRoughness;

        SSRPass = CreateShared<SSR>(SCR_WIDTH, SCR_HEIGHT, SSRInputs, SSROutputs);
    }

    //=========================================

    //=====skybox pass
    skyboxShader = Shader::Create("skybox shader",
                                  "Shaders/Shaders/Skybox_VS.glsl",
                                  "Shaders/Shaders/Skybox_FS.glsl");

    auto skyboxMaterial = Material::Create("skyboxMaterial");
    skyboxMaterial->SetTexture(TexType::SkyboxTexture, origin_envMap);
    // skyboxMaterial->SetTexture(TexType::SkyboxTexture, diffuseEnvMap);

    skybox       = CreateActor<Cube>();
    skybox->Name = "skybox";
    skybox->SetShader(skyboxShader);
    skybox->SetMaterial(skyboxMaterial);

    //
    screenQuadShader        = Shader::Create("screen quad shader",
                                      "Shaders/Shaders/ScreenQuad_VS.glsl",
                                      "Shaders/Shaders/ScreenQuad_FS.glsl");
    auto screenQuadMaterial = Material::Create("screenQuadMaterial");

    screenQuad       = CreateActor<ScreenQuad>();
    screenQuad->Name = "screenQuad";
    screenQuad->SetShader(screenQuadShader);
    screenQuad->SetMaterial(screenQuadMaterial);
}

//====================================
//====================================
void DeferredPBR::OnUpdate(float deltaTime)
{
    timer += deltaTime;

    // tick
    for (auto& _actor : staticMeshActors)
    {
        _actor->Tick(deltaTime);
    }

    main_camera->Tick(deltaTime);

    sunlight->SunlightComponent->SetDirection(glm::vec3(glm::cos(timer), -1.0f, glm::sin(timer)));
    sunlight->Tick(deltaTime);
    skybox->Tick(deltaTime);

    //====== shadowMap pass: render the scene to the shadowMap;
    {
        shadowMapFBO->Bind();
        glClear(GL_DEPTH_BUFFER_BIT); // make sure clear the framebuffer's content
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glEnable(GL_DEPTH_TEST);

        shadowMapShader->Bind();

        sunlight->SunlightComponent->SetShadowPassState(shadowMapShader);
        // auto _view = sunlight->SunlightComponent->LightSpaceCamera->GetViewMatrix();
        // auto _proj = sunlight->SunlightComponent->LightSpaceCamera->GetProjectionMatrix();
        // shadowMapShader->SetMat4("u_view", _view);
        // shadowMapShader->SetMat4("u_projection", _proj);

        for (auto& _meshObj : staticMeshActors)
        {
            _meshObj->StaticMeshComponent->SetGeometryState(shadowMapShader);
            _meshObj->StaticMeshComponent->Draw();
        }

        this->shadowMapShader->Unbind();
        shadowMapFBO->Unbind();
    }

    //======gBuffer pass: render the scene to the gbuffer
    {
        gBufferFBO->Bind();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // make sure clear the framebuffer's content
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glEnable(GL_DEPTH_TEST);

        for (auto& _meshObj : staticMeshActors)
        {
            auto _shader = _meshObj->StaticMeshComponent->GetShader();
            _shader->Bind();

            main_camera->CameraComponent->SetRenderState(gBufferPassShader);
            _meshObj->StaticMeshComponent->SetGeometryState(_shader);
            _meshObj->StaticMeshComponent->SetMaterialState(_shader);
            _meshObj->StaticMeshComponent->Draw();
        }

        gBufferFBO->Unbind();
    }

    //======WorldToView pass
    {
        WorldToViewPass->Render(main_camera);
    }

    //======Lit pass
    {
        litPassFBO->Bind();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);

        litPassShader->Bind();

        // camera info
        litPassShader->SetVec3("u_CameraPos", main_camera->CameraComponent->GetWorldPosition());

        // light info
        sunlight->SunlightComponent->SetLitPassState(litPassShader);

        litPassShader->SetBool("u_EnableSkyBox", enableSkyBox);

        litPassShader->SetFloat("u_min_shadow_bias", min_shadow_bias);
        litPassShader->SetFloat("u_max_shadow_bias", max_shadow_bias);

        litPassQuad->DrawSelfContained();

        // glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        litPassFBO->Unbind();
    }

    //=======postprocessing
    if (enableSSAO)
    {
        // RD_PROFILE_SCOPE("SSAO pass");
        SSAOPass->Render(main_camera);
    }

    if (enableBloom)
    {
        BloomPass->Render();
    }

    if (enableOutline)
    {
        OutlinePass->Render();
    }

    if (enableSSR)
    {
        SSRPass->Render(main_camera);
    }

    //=======output any texture;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    screenQuadShader->Bind();
    visualizeBuffer = litOutputs[TexType::ScreenTexture];
    // visualizeBuffer     = shadowMap;
    auto gbuffertexture = gBufferFBO->GetColorTextures();
    // visualizeBuffer     = debugHDRI;
    //  visualizeBuffer     = gbuffertexture[TexType::gWorldPosition];
    //  visualizeBuffer = gbuffertexture[TexType::gAlbedo];
    if (visualizeBuffer != nullptr)
    {
        screenQuadShader->SetBool("u_isGrayScale", visualizeBuffer->isGrayScale());
        screenQuadShader->SetFloat("u_mipLevel", bufferMipLevel);

        // screenQuadShader->SetBool("u_isDepthTexture", true);
        // screenQuadShader->SetFloat("u_near_plane", sunlight->SunlightComponent->LightSpaceCamera->nearPlane);
        // screenQuadShader->SetFloat("u_far_plane", sunlight->SunlightComponent->LightSpaceCamera->farPlane);

        glBindTextureUnit(0, visualizeBuffer->GetID());
    }

    screenQuad->DrawSelfContained();
    screenQuadShader->Unbind();

    //=======skybox overlay;
    // this is final pass of scene rendering;
    // compare the depth with gbuffer;  make sure enable the depth test;

    if (enableSkyBox)
    {
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBO->GetFrameBufferID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        auto _skyboxShader = skybox->StaticMeshComponent->GetShader();
        _skyboxShader->Bind();
        //_skyboxShader->SetFloat("u_mipLevel", bufferMipLevel);

        auto _view = main_camera->CameraComponent->GetViewMatrix();
        // remove the translation part of the view matrix
        _view = glm::mat4(glm::mat3(_view));

        _skyboxShader->SetMat4("u_view", _view);
        _skyboxShader->SetMat4("u_projection", main_camera->CameraComponent->GetProjectionMatrix());

        skybox->StaticMeshComponent->SetGeometryState(_skyboxShader);
        skybox->StaticMeshComponent->SetMaterialState(_skyboxShader);

        skybox->DrawSkybox();

        _skyboxShader->Unbind();
    }

    //==========
    // buffer visualization
    if (visualize_gbuffer)
    {
        // render the buffers to the screen, for debugging
        // WARN: don't clear the buffer for overlay;
        glDisable(GL_DEPTH_TEST);

        std::vector<std::pair<int, int>> leftBottom = {
            { 0, 0 },
            { 1, 0 },
            { 2, 0 },
            { 3, 0 },
            { 0, 3 },
            { 1, 3 },
            { 2, 3 },
            { 3, 3 },
        };

        auto gbuffer_tex = gBufferFBO->GetColorTextures();

        std::vector<SharedPtr<UTexture>> bufferTextures = {
            gbuffer_tex[TexType::gWorldPosition],
            gbuffer_tex[TexType::gWorldNormal],
            gbuffer_tex[TexType::gWorldTangent],
            gbuffer_tex[TexType::gAlbedo],
            gbuffer_tex[TexType::gMetallic],
            gbuffer_tex[TexType::gRoughness],
            // gViewDepth ,
            //  gViewPosition,
            //  gViewNormal,
            //  litPassScreenTexture,
            shadowMap,
        };

        int index = 0;
        for (auto& g_texture : bufferTextures)
        {
            if (g_texture == nullptr)
                continue;
            if (index >= 8)
                break;
            screenQuadShader->Bind();

            glViewport(leftBottom[index].first * BUFFER_WIDTH, leftBottom[index].second * BUFFER_HEIGHT, BUFFER_WIDTH, BUFFER_HEIGHT);
            glBindTextureUnit(0, g_texture->GetID());
            screenQuadShader->SetBool("u_isGrayScale", g_texture->isGrayScale());

            screenQuad->DrawSelfContained();
            index++;
        }
        glEnable(GL_DEPTH_TEST);
    }

    //======gui
    // this->RenderGUI();

    // RendererApp::WindowOnUpdate();
    /* Swap front and back buffers */
    // glfwSwapBuffers(window);
    /* Poll for and process events */
    // glfwPollEvents();

    //====shutdown
    // this->ShutDownGUI();
    // glfwTerminate();

} // onUpdate

void DeferredPBR::OnImGuiRender()
{
    // 渲染GUI
    ImGui::Begin("Rudy Engine");

    ImGui::Text("Hello World");
    ImGui::Checkbox("enableSkybox", &enableSkyBox);

    std::unordered_map<std::string, SharedPtr<UTexture>> bufferList = {
        { "Lit Pass output", litOutputs[TexType::ScreenTexture] },
        { "SSR output", SSROutputs[TexType::ScreenTexture] },
        { "SSAO output", SSAOOutputs[TexType::ScreenTexture] },
        { "Outline output", OutlineOutputs[TexType::ScreenTexture] },
        //{ "Composer output",  ComposerOutputs[TexType::ScreenTexture]},
        { "Bloom output", BloomOutputs[TexType::ScreenTexture] },
        { "tone map output", ToneMapOutputs[TexType::ScreenTexture] },

        { "sunlight shadowmap", shadowMap },
    };

    // 创建一个保存所有键的字符串数组
    std::vector<std::string> keys = {
        "Lit Pass output",
        "SSR output",
        "SSAO output",
        "Outline output",
        //"Composer output",
        "Bloom output",
        "tone map output",
        "sunlight shadowmap",
    };

    static int  item_current_idx = 0;  // 如果你有一个可以从外部访问的默认项，也可以在这里设置它的索引
    std::string item_current     = ""; // 用于显示和选择当前项

    if (bufferList.size() > 0)
    {
        // 获取当前选中项的键
        item_current    = keys[item_current_idx];
        visualizeBuffer = bufferList[item_current];

        if (ImGui::BeginCombo("Visualize Buffer", item_current.c_str()))
        {
            for (int n = 0; n < keys.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(keys[n].c_str(), is_selected))
                {
                    item_current_idx = n;
                    visualizeBuffer  = bufferList[keys[n]];
                }

                // 设置初始焦点项
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }

    ImGui::Checkbox("enableSSAO", &enableSSAO);
    ImGui::Checkbox("enableSSAOBlur", &(SSAOPass->enableBlur));
    ImGui::Checkbox("enableBloom", &enableBloom);
    ImGui::Checkbox("enableOutline", &enableOutline);
    // ImGui::Checkbox("enableSSR", &enableSSR);

    // group: SSAO

    ImGui::Spacing();
    ImGui::SliderFloat("SSAO radius", &(SSAOPass->radius), 0.0f, 1.0f);
    ImGui::SliderFloat("SSAO bias", &(SSAOPass->bias), 0.0f, 1.0f);
    ImGui::SliderFloat("SSAO range bias", &(SSAOPass->range_bias), 0.0f, 1.0f);
    ImGui::SliderInt("SSAO kernel size", &(SSAOPass->kernelSize), 0, 64);

    ImGui::Spacing();
    ImGui::SliderFloat("Bloom threshold", &(BloomPass->threshold), 0.0f, 1.0f);
    ImGui::SliderFloat("Bloom Strength", &(BloomPass->bloom_strength), 0.0f, 3.0f);
    ImGui::SliderFloat("Bloom radius", &(BloomPass->bloom_radius), 0.0f, 1.0f);

    ImGui::Spacing();
    ImGui::SliderFloat("outline_width", &(OutlinePass->outline_width), 1.0f, 5.0f);
    ImGui::SliderFloat("outline depth_thres", &(OutlinePass->depth_thres), 0.001f, 1.0f);
    ImGui::SliderFloat("outline depth_thres_scale", &(OutlinePass->depth_thres_scale), 0.001f, 20.0f);
    ImGui::SliderFloat("outline depth_NdotV_threshold", &(OutlinePass->depth_NdotV_threshold), 0.0f, 1.0f);
    ImGui::SliderFloat("outline normal_threshold", &(OutlinePass->normal_threshold), 0.01f, 1.0f);

    ImGui::Checkbox("outline enable depth", &(OutlinePass->enable_depth));
    ImGui::Checkbox("outline enable normal", &(OutlinePass->enable_normal));

    ImGui::Spacing();
    ImGui::SliderFloat("SSR depth_bias", &(SSRPass->depth_bias), 0.0f, 0.04f);
    ImGui::SliderFloat("SSR step_size", &(SSRPass->step_size), 0.01f, 0.3f);
    ImGui::SliderInt("SSR max_steps", &(SSRPass->max_steps), 50, 300);
    ImGui::Checkbox("SSR enableBlur", &(SSRPass->enableBlur));

    ImGui::Spacing();
    ImGui::SliderFloat("Lit min_shadow_bias", &(min_shadow_bias), 0.00f, 0.2f);
    ImGui::SliderFloat("Lit max_shadow_bias", &(max_shadow_bias), 0.00f, 0.2f);

    ImGui::SliderFloat("Buffer miplevel", &bufferMipLevel, 0, 10);

    ImGui::End();

    // ShowHierarchy();
}

} // namespace Rudy