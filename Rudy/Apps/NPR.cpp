// update 2024.1.22
#include "RudyPCH.h"
#include "NPR.h"

namespace Rudy
{

NPR::NPR() :
    Application()
{
    // this->GetWindow()->SetVSync(false);
}

Ref<NPR> NPR::Create()
{
    return CreateRef<NPR>();
}

void NPR::Init()
{
}

void NPR::Start()
{
    Rudy::Log::Init();
    RD_CORE_WARN("test:Initialized Log!");

    //========================================
    //=== initialize renderer
    // auto window = Window::Create();
    // Input::SetWindowContext(window->GetNativeWindow());

    main_camera = Camera::Create(MAIN_CAMERA_POS);

    RendererApp::Init(SCR_WIDTH, SCR_HEIGHT);
    RendererApp::SetMainCamera(main_camera);

    window         = RendererApp::GetWindow();
    auto renderAPI = RendererApp::GetAPI();

    // gui
    this->InitGUI();

    //=================================================================================================
    //=== initialize the resources

    //==========shadow map pass
    auto shadowMapShader = Shader::Create("shadowMap Shader",
                                          "Shaders/Shaders/DepthMap_VS.glsl",
                                          "Shaders/Shaders/DepthMap_FS.glsl");
    // Material::SetMaterialProperties(shadowMapShader);

    auto shadowMapSkinnedShader = Shader::Create("shadowMap skinned Shader",
                                                 "Shaders/Shaders/DepthMapSkinned_VS.glsl",
                                                 "Shaders/Shaders/DepthMap_FS.glsl");
    // Material::SetMaterialProperties(shadowMapSkinnedShader);

    auto shadowMapMaterial        = Material::Create(shadowMapShader);
    auto shadowMapSkinnedMaterial = Material::Create(shadowMapSkinnedShader);

    auto shadowMapFBO = FrameBuffer::Create("shadowMap FBO",
                                            SHADOW_WIDTH,
                                            SHADOW_HEIGHT,
                                            FrameBufferType::DepthTexture);

    shadowMap = Texture2D::CreateEmpty(
        TextureSpec { SHADOW_WIDTH, SHADOW_HEIGHT, TextureInternalFormat::DEPTH_COMPONENT24, false, WrapMode::ClampToBorder, FilterMode::Nearest });
    // set the border color for the depth texture;

    float borderColor2[] = { 1.0, 1.0, 1.0, 1.0 };
    glTextureParameterfv(shadowMap->GetID(), GL_TEXTURE_BORDER_COLOR, borderColor2);

    shadowMapFBO->SetDepthTexture(shadowMap);
    // shadowMapFBO->FinishSetup();

    //======gbuffer pass;
    // the framebuffer for gbuffer pass: 8;
    // auto gWorldPosition = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
    // auto gWorldNormal = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
    // auto gWorldTangent = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
    //
    // auto gAlbedo = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
    // auto gSpecular = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
    // auto gMetallic = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
    auto gRoughness = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });

    std::cout << "lit pass" << std::endl;
    //===============================
    //======lit pass
    auto  gViewDepth    = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::DEPTH_COMPONENT24, false, WrapMode::ClampToBorder, FilterMode::Nearest });
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTextureParameterfv(gViewDepth->GetID(), GL_TEXTURE_BORDER_COLOR, borderColor);

    // new: extension of gbuffers;
    auto gViewPosition = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
    auto gViewNormal   = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });

    auto litPassScreenTexture = Texture2D::CreateEmpty(
        TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F, false, WrapMode::ClampToBorder, FilterMode::Linear, FilterMode::Linear });

    auto litPassShader = Shader::Create("NPR Shader",
                                        "Shaders/Shaders/Genshin_VS.glsl",
                                        "Shaders/Shaders/Genshin_FS.glsl");
    // "Shaders/Shaders/default_VS.glsl", "Shaders/Shaders/default_FS.glsl");
    Material::SetMaterialProperties(litPassShader);

    //=== FBO
    litPassFBO = FrameBuffer::Create("litPass FBO",
                                     SCR_WIDTH,
                                     SCR_WIDTH,
                                     FrameBufferType::Regular);

    litPassFBO->SetColorTexture(TexType::ScreenTexture, litPassScreenTexture, 0);
    litPassFBO->SetColorTexture(TexType::gViewPosition, gViewPosition, 1);
    litPassFBO->SetColorTexture(TexType::gViewNormal, gViewNormal, 2);

    litPassFBO->SetDepthTexture(gViewDepth);
    litPassFBO->FinishSetup();

    litOutputs[TexType::ScreenTexture] = litPassScreenTexture;

    std::cout << "scene description" << std::endl;
    //=====the scene
    // auto scene = Scene::Create();

    // lighting
    auto sunlight       = DirectionalLight::Create();
    sunlight->intensity = 1.0f;
    sunlight->direction = glm::vec3(0.5f, -0.5f, -1.0f);

    // info for shadowmap:
    // the orthographic projection matrix for the light source：
    float     near_plane = -5.0f, far_plane = 5.0f;
    glm::mat4 lightProjection = glm::ortho(
        -5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane); // the near and far plane should be large enough to cover the scene
    // look at minus direction;
    glm::mat4 lightView = glm::lookAt(-sunlight->direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

    auto lightSpaceCamera                = Camera::Create();
    lightSpaceCamera->m_ProjectionMatrix = lightProjection;
    lightSpaceCamera->m_ViewMatrix       = lightView;

    //=======actors

    auto floor_gMaterial = NPRMaterial::Create(litPassShader);

    floor_gMaterial->SetVec3("u_Albedo", glm::vec3(0.5));
    // auto floor_albedoMap =    Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/albedo.png");
    auto floor_normalMap = Texture2D::LoadFile("Resources/PBRTextures/Floor_brown_ue/normal.png");
    // auto floor_roughnessMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/roughness.png");
    // auto floor_metallicMap =  Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/metallic.png");

    // floor_gMaterial->SetTexture(TexType::AlbedoMap, floor_albedoMap);
    floor_gMaterial->SetTexture(TexType::NormalMap, floor_normalMap);
    // floor_gMaterial->SetTexture(TexType::RoughnessMap, floor_roughnessMap);
    // floor_gMaterial->SetTexture(TexType::MetallicMap, floor_metallicMap);

    floor_gMaterial->SetTexture(TexType::DepthTexture, shadowMap);

    auto floor1 = Plane::Create(10);
    floor1->SetMaterial(floor_gMaterial);
    floor1->transform->scale = glm::vec3(5.0f);
    staticMeshObjects.push_back(floor1);

    //
    auto sphere_gMaterial = NPRMaterial::Create(litPassShader);
    // sphere_gMaterial->SetVec3("u_Albedo", glm::vec3(1.0f, 0.0f, 0.0f));

    sphere_gMaterial->SetVec3("u_Albedo", glm::vec3(1.0f, 0.0f, 1.0f));

    sphere_gMaterial->SetBool("u_receive_shadow", false);

    // auto sphere_albedoMap =    Texture2D::LoadFile("Resources/PBRTextures/rusted_iron_ue/albedo.png");
    // auto sphere_normalMap =    Texture2D::LoadFile("Resources/PBRTextures/rusted_iron_ue/normal.png");
    // auto sphere_roughnessMap = Texture2D::LoadFile("Resources/PBRTextures/rusted_iron_ue/roughness.png");
    // auto sphere_metallicMap =  Texture2D::LoadFile("Resources/PBRTextures/rusted_iron_ue/metallic.png");

    // sphere_gMaterial->SetTexture(TexType::AlbedoMap,    sphere_albedoMap);
    // sphere_gMaterial->SetTexture(TexType::NormalMap,    sphere_normalMap);
    // sphere_gMaterial->SetTexture(TexType::RoughnessMap, sphere_roughnessMap);
    // sphere_gMaterial->SetTexture(TexType::MetallicMap,  sphere_metallicMap);

    sphere_gMaterial->SetTexture(TexType::DepthTexture, shadowMap);

    auto sphere = Sphere::Create(20);
    sphere->SetMaterial(sphere_gMaterial);
    sphere->transform->position = glm::vec3(2.0f, +1.0f, 0.0f);

    // staticMeshObjects.push_back(sphere);

    std::cout << "model loading" << std::endl;
    // model
    // Model::ScaleFactor = 0.01f;
    //  auto test_model = Model::LoadModel("D:/CG_resources/animation/Catwalk Walk Turn 180 Tight.dae");

    // auto test_model = Model::LoadModel("D:/CG_resources/backpack/backpack.obj");
    // auto model = Model::LoadModel("D:/CG_resources/dae/vampire/dancing_vampire.dae");

    //====material
    Texture2D::SetFlipYOnLoad(true); // eg: for .png;
    auto faceSDF = Texture2D::LoadFile("Resources/faceLightMap/Avatar_Girl_Tex_FaceLightmap.png");

    // model loading
    Texture2D::SetFlipYOnLoad(true); // eg: for .png;
    Model::s_scaleFactor = 0.01f;
    auto model           = Model::LoadModel("D:/CG_resources/blender_export/player_no_bones.dae");
    this->models.push_back(model);

    // for now: assume model use same shader.
    model->shader = litPassShader;

    // set animator
    if (model->hasAnimation())
    {
        auto animator = Animator::Create(model);
        model->SetAnimator(animator);
    }

    for (auto meshObj : model->meshObjects)
    {
        auto old_material = meshObj->GetRenderer()->GetMaterial();
        old_material->SetTexture(TexType::FaceLightMap, faceSDF);
        old_material->SetTexture(TexType::DepthTexture, shadowMap);

        // switch textures to the new material;
        auto new_material = NPRMaterial::Create(litPassShader);
        auto textures     = old_material->GetTextures();
        new_material->SetTextures(textures);
        meshObj->SetMaterial(new_material);

        new_material->SetBool("u_receive_shadow", false);

        meshObj->transform->rotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        meshObj->transform->position = glm::vec3(0.0f, 0.0f, 2.0f);
    }

    // face is material 1
    auto faceMaterial = model->meshObjects[0]->GetRenderer()->GetMaterial();
    faceMaterial->SetBool("u_face", true);
    faceMaterial->SetBool("u_skipNormal", true);

    auto eyeMaterial = model->meshObjects[1]->GetRenderer()->GetMaterial();
    eyeMaterial->SetBool("u_skipNormal", true);

    //=========
    std::cout << "postprocessing" << std::endl;

    // SSAO
    {
        SSAOInputs[TexType::gViewPosition] = gViewPosition;
        SSAOInputs[TexType::gViewNormal]   = gViewNormal;
        // SSAOInputs[TexType::gWorldTangent] = gWorldTangent;
        SSAOInputs[TexType::gViewDepth] = gViewDepth;

        SSAOPass = CreateRef<SSAO>(SCR_WIDTH, SCR_HEIGHT, SSAOInputs, SSAOOutputs);
    }

    // Outline
    {
        OutlineInputs[TexType::gViewPosition] = gViewPosition;
        OutlineInputs[TexType::gViewNormal]   = gViewNormal;

        OutlinePass = CreateRef<Outline>(SCR_WIDTH, SCR_HEIGHT, OutlineInputs, OutlineOutputs);
    }

    // SSR
    {
        SSRInputs[TexType::gViewPosition] = gViewPosition;
        SSRInputs[TexType::gViewNormal]   = gViewNormal;
        SSRInputs[TexType::ScreenTexture] = litPassScreenTexture;
        SSRInputs[TexType::gViewDepth]    = gViewDepth;

        SSRInputs[TexType::gRoughness] = gRoughness;

        SSRPass = CreateRef<SSR>(SCR_WIDTH, SCR_HEIGHT, SSRInputs, SSROutputs);
    }

    // composer
    {
        ComposerInputs[TexType::litPassTexture]       = litPassScreenTexture;
        ComposerInputs[TexType::SSROutputTexture]     = SSROutputs[TexType::ScreenTexture];
        ComposerInputs[TexType::OutlineOutputTexture] = OutlineOutputs[TexType::ScreenTexture];

        ComposerPass = CreateRef<Composer>(SCR_WIDTH, SCR_HEIGHT, ComposerInputs, ComposerOutputs);
    }

    // Bloom
    {
        BloomInputs[TexType::ScreenTexture] = litPassScreenTexture;

        BloomPass = CreateRef<Bloom>(SCR_WIDTH, SCR_HEIGHT, BloomInputs, BloomOutputs);
    }

    // tone mapping
    {
        ToneMapInputs[TexType::ScreenTexture] = litPassScreenTexture;

        ToneMapPass = CreateRef<ToneMap>(SCR_WIDTH, SCR_HEIGHT, ToneMapInputs, ToneMapOutputs);
    }

    //=========================================

    //=====skybox pass
    auto skyboxShader = Shader::Create("skybox",
                                       "Shaders/Shaders/Skybox_VS.glsl",
                                       "Shaders/Shaders/Skybox_FS.glsl");
    Material::SetMaterialProperties(skyboxShader);

    //
    auto      gridShader = Shader::Create("default Shader", "Shaders/Shaders/Default_VS.glsl", "Shaders/Shaders/Default_Flat_FS.glsl");
    WorldGrid grid       = WorldGrid(20);
    grid.material        = Material::Create(gridShader);

    auto       lineShader = Shader::Create("vertex color Shader", "Shaders/Shaders/Vertex_Color_VS.glsl", "Shaders/Shaders/Vertex_Color_FS.glsl");
    Navigation nav        = Navigation();
    nav.material          = Material::Create(lineShader);

    auto screenQuadShader   = Shader::Create("screen quad shader",
                                           "Shaders/Shaders/ScreenQuad_VS.glsl",
                                           "Shaders/Shaders/ScreenQuad_FS.glsl");
    auto screenQuadMaterial = Material::Create(screenQuadShader);

    auto screenQuad = ScreenQuad::Create();
    screenQuad->SetMaterial(screenQuadMaterial);

    //======the loop
    /* Loop until the user closes the window */

    float lastFrameTime = 0.0f;
    float timer         = 0.0f;
    RD_CORE_WARN("App: Entering the loop");
    while (!RendererApp::ShouldClose())
    {
        // RD_PROFILE_SCOPE("the game loop");

        // get the time of each frame
        float time      = (float)glfwGetTime();
        float deltaTime = time - lastFrameTime;
        lastFrameTime   = time;
        timer += deltaTime;

        //'gui'
        this->PrepareGUI();

        // routined updates
        // animations
        {
            if (model->animator != nullptr)
            {
                model->animator->UpdateBoneTransforms(timer);
                auto transforms = model->animator->GetBoneTransforms();
                model->boneTransformBuffer->SetData(transforms.data(), transforms.size() * sizeof(glm::mat4));
            }
        }

        //====== shadowMap pass: render the scene to the shadowMap;
        if (true)
        {
            shadowMapFBO->Bind();
            glClear(GL_DEPTH_BUFFER_BIT); // make sure clear the framebuffer's content
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glEnable(GL_DEPTH_TEST);

            for (auto& _meshObj : staticMeshObjects)
            {
                _meshObj->GetRenderer()->Draw(lightSpaceCamera, 1, shadowMapMaterial);
            }

            for (auto& _model : models)
            {
                _model->Draw(lightSpaceCamera, 1, shadowMapSkinnedMaterial);
            }

            shadowMapFBO->Unbind();
        }

        //======Lit pass
        if (true)
        {
            litPassFBO->Bind();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // make sure clear the framebuffer's content
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glEnable(GL_DEPTH_TEST);

            // scene info
            litPassShader->Bind();

            // light info
            litPassShader->SetVec3("u_DirLight.direction", direct_light_dir); // sunlight->direction);
            litPassShader->SetVec3("u_DirLight.color", sunlight->color);
            litPassShader->SetFloat("u_DirLight.intensity", direct_light_intensity); // sunlight->intensity);

            litPassShader->SetMat4("u_LightSpaceMatrix", lightSpaceCamera->GetProjectionViewMatrix());

            // camera info
            litPassShader->SetVec3("u_CameraPos", main_camera->GetPosition());

            // litPassShader->SetBool("u_EnableSkyBox", enableSkyBox);

            litPassShader->SetFloat("u_min_shadow_bias", min_shadow_bias);
            litPassShader->SetFloat("u_max_shadow_bias", max_shadow_bias);

            litPassShader->SetVec3("u_faceForward", glm::vec3(0.0f, 0.0f, 1.0f));
            litPassShader->SetVec3("u_faceRight", glm::vec3(-1.0f, 0.0f, 0.0f));

            litPassShader->SetVec3("u_litColor", litColor);
            litPassShader->SetVec3("u_shadowColor", shadowColor);

            litPassShader->SetFloat("u_diffuse_cutoff", diffuse_cutoff);

            litPassShader->SetFloat("u_ambient_coeff", ambient_coeff);

            for (auto& _meshObj : staticMeshObjects)
            {
                _meshObj->GetRenderer()->Draw(main_camera);
            }

            for (auto& _model : models)
            {
                _model->Draw(main_camera);
            }

            litPassFBO->Unbind();
            glDisable(GL_DEPTH_TEST);
        }

        //=======postprocessing
        if (enableSSAO)
        {
            // RD_PROFILE_SCOPE("SSAO pass");
            SSAOPass->Render(main_camera);
        }

        if (enableOutline)
        {
            OutlinePass->Render(main_camera);
        }

        if (enableSSR)
        {
            SSRPass->Render(main_camera);
        }

        if (enableComposer)
        {
            ComposerPass->Render(main_camera);
        }

        if (enableBloom)
        {
            BloomPass->Render(main_camera);
        }

        if (enableToneMap)
        {
            ToneMapPass->Render(main_camera);
        }

        //=======output any texture;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        screenQuadShader->Bind();
        // visualizeBuffer = BloomOutputs[TexType::ScreenTexture];
        // visualizeBuffer = SSAOOutputs[TexType::ScreenTexture];
        if (visualizeBuffer != nullptr)
        {
            int channel = visualizeBuffer->GetChannels();
            screenQuadShader->SetBool("u_isGrayScale", channel == 1);
            screenQuadShader->SetFloat("u_mipLevel", bufferMipLevel);

            // screenQuadShader->SetBool("u_isDepthTexture", true);
            // screenQuadShader->SetFloat("u_near_plane", near_plane);
            // screenQuadShader->SetFloat("u_far_plane", far_plane);

            glBindTextureUnit(0, visualizeBuffer->GetID());
        }

        screenQuad->Draw(nullptr);
        screenQuadShader->Unbind();

        //=======skybox overlay;
        // this is final pass of scene rendering;
        // compare the depth with gbuffer;  make sure enable the depth test;

        // if (enableSkyBox)
        // {
        //     glEnable(GL_DEPTH_TEST);
        //     glBindFramebuffer(GL_READ_FRAMEBUFFER, GBufferFBO->GetFrameBufferID());
        //     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        //     glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        //
        //     skyboxShader->Bind();
        //     skyboxShader->SetFloat("u_mipLevel", bufferMipLevel);
        //     skybox.DrawSkybox(main_camera);
        //
        //     skyboxShader->Unbind();
        //
        // }

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

            std::vector<Ref<Texture>> bufferTextures = {
                // gWorldPosition,
                // gWorldNormal,
                ////gWorldTangent,
                // gAlbedo,
                // gSpecular,
                // gMetallic,
                // gRoughness,
                gViewDepth,
                gViewPosition,
                gViewNormal,
                litPassScreenTexture,
                shadowMap,
            };

            int index = 0;
            for (auto& g_texture : bufferTextures)
            {
                if (index >= 8)
                    break;
                screenQuadShader->Bind();

                glViewport(leftBottom[index].first * BUFFER_WIDTH, leftBottom[index].second * BUFFER_HEIGHT, BUFFER_WIDTH, BUFFER_HEIGHT);
                glBindTextureUnit(0, g_texture->GetID());
                int channel = g_texture->GetChannels();
                screenQuadShader->SetBool("u_isGrayScale", channel == 1);

                screenQuad->Draw(nullptr);
                index++;
            }
            glEnable(GL_DEPTH_TEST);
        }

        //=======overlays
        // grid.Draw();
        nav.Draw();

        main_camera->OnUpdate(deltaTime);

        //======gui
        this->RenderGUI();
        RendererApp::WindowOnUpdate();
        /* Swap front and back buffers */
        // glfwSwapBuffers(window);
        /* Poll for and process events */
        // glfwPollEvents();
    }

    //====shutdown
    this->ShutDownGUI();
    glfwTerminate();
}

void NPR::DrawGUI()
{
    // 渲染GUI
    ImGui::Begin("Rudy Engine");
    ImGui::Text("Hello World");

    ImGui::Checkbox("enableSkybox", &enableSkyBox);

    std::unordered_map<std::string, Ref<Texture>> bufferList = {
        { "Lit Pass output", litOutputs[TexType::ScreenTexture] },
        { "SSR output", SSROutputs[TexType::ScreenTexture] },
        { "SSAO output", SSAOOutputs[TexType::ScreenTexture] },
        { "Outline output", OutlineOutputs[TexType::ScreenTexture] },
        { "Composer output", ComposerOutputs[TexType::ScreenTexture] },
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
        "Composer output",
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

    ImGui::Checkbox("enableSSR", &enableSSR);
    ImGui::Checkbox("enableOutline", &enableOutline);

    ImGui::Checkbox("enableComposer", &enableComposer);

    ImGui::Checkbox("enableBloom", &enableBloom);
    ImGui::Checkbox("enableToneMap", &enableToneMap);

    // group: SSAO

    ImGui::Spacing();
    ImGui::SliderFloat("SSAO radius", &(SSAOPass->radius), 0.0f, 1.0f);
    ImGui::SliderFloat("SSAO bias", &(SSAOPass->bias), 0.0f, 1.0f);
    ImGui::SliderFloat("SSAO range bias", &(SSAOPass->range_bias), 0.0f, 1.0f);
    ImGui::SliderInt("SSAO kernel size", &(SSAOPass->kernelSize), 0, 64);

    ImGui::Spacing();
    ImGui::SliderFloat("Bloom threshold", &(BloomPass->threshold), 0.0f, 5.0f);
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

    ImGui::Spacing();
    ImGui::SliderFloat("ambient_coeff", &(ambient_coeff), 0.0f, 5.0f);
    ImGui::SliderFloat("dirlight_intensity", &(direct_light_intensity), 0.0f, 5.0f);
    ImGui::InputFloat3("dirLight_direction", glm::value_ptr(direct_light_dir));

    ImGui::InputFloat3("lit color", glm::value_ptr(litColor));
    ImGui::InputFloat3("shadow color", glm::value_ptr(shadowColor));

    ImGui::SliderFloat("diffuse_cutoff", &(diffuse_cutoff), 0.0f, 1.0f);

    ImGui::Spacing();
    ImGui::SliderFloat("toneMap exposure", &(ToneMapPass->exposure), 0.0f, 10.0f);

    ImGui::Spacing();
    ImGui::SliderFloat("composer outline_scale", &(ComposerPass->outline_scale), 0.0f, 1.0f);
    ImGui::SliderFloat("composer SSR_scale", &(ComposerPass->SSR_scale), 0.0f, 1.0f);

    ImGui::SliderFloat("Buffer miplevel", &bufferMipLevel, 0, 10);
    ImGui::End();
}

void NPR::InitGUI()
{
    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // 设置ImGui的样式
    ImGui::StyleColorsDark();
    // 绑定后端
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)this->window->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // other settings
    ImGui::SetNextWindowSize(ImVec2(500, 400)); // 设置窗口大小为 500x400

    // initial position on the right top corner;
    ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH - 500, 0)); // 设置窗口位置为 (SCR_WIDTH - 500, 0)

    ImGui::CreateContext();
    ImGuiIO&     io = ImGui::GetIO();
    ImFontConfig fontConfig;
    fontConfig.SizePixels = 18.0f; // 设置字体大小为 18 像素
    io.Fonts->AddFontDefault(&fontConfig);
}

} // namespace Rudy
