// update 2024.1.22
#include "RudyPCH.h"
#include "PBR.h"

namespace Rudy
{

PBR::PBR() :
    ViewportLayer()
{
}

void PBR::ShutDown()
{
    // all lifetime management is done by the smart pointers;
}

void PBR::Init()
{
    // impose: init base class;
    ViewportLayer::Init();

    //=================================================================================================
    //=== initialize resources

    //=======environment map precomputing pass

    Texture2D::SetFlipYOnLoad(true);
    // auto testHDRI = Texture2D::LoadFile("D:/CG_resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr");

    auto envMap = TextureCube::LoadHDRI("Resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr");

    // if no precomputed envmap available
    auto diffuseEnvMap = TextureCube::LoadHDRI("Resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_Env.hdr");

    // lod 5 for specular envmap;
    auto specularEnvMap = envMap->CreatePrefilteredEnvMap(envMap,
                                                          ConvolutionType::Specular,
                                                          5);

    // BRDF integration map
    auto brdfLUTShader = Shader::Create("brdfLUT Shader", "Shaders/Shaders/BrdfLUT_VS.glsl", "Shaders/Shaders/BrdfLUT_FS.glsl");
    Material::SetMaterialProperties(brdfLUTShader);

    auto brdfQuadMaterial = Material::Create(brdfLUTShader);

    // don't forget to RGBA if ever use as image2D
    auto brdfLUTTexture = Texture2D::CreateEmpty(
        TextureSpec { 512, 512, TextureInternalFormat::RGB32F });

    auto brdfLUTFBO = FrameBuffer::Create("brdfLUT FBO", 512, 512, FrameBufferType::Regular);

    brdfLUTFBO->Bind();
    brdfLUTFBO->SetColorTexture(TexType::brdfLUTTexture, brdfLUTTexture, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, 512, 512);

    auto brdfQuad = ScreenQuad::Create();
    brdfQuad->SetMaterial(brdfQuadMaterial);
    brdfQuad->Draw(nullptr);
    brdfLUTFBO->Unbind();

    //==========shadow map pass
    auto shadowMapShader = Shader::Create("shadowMap Shader",
                                          "Shaders/Shaders/DepthMap_VS.glsl",
                                          "Shaders/Shaders/DepthMap_FS.glsl");
    // Material::SetMaterialProperties(shadowMapShader);

    auto shadowMapSkinnedShader = Shader::Create("shadowMap skinned Shader",
                                                 "Shaders/Shaders/DepthMapSkinned_VS.glsl",
                                                 "Shaders/Shaders/DepthMap_FS.glsl");
    // Material::SetMaterialProperties(shadowMapSkinnedShader);

    shadowMapMaterial        = Material::Create(shadowMapShader);
    shadowMapSkinnedMaterial = Material::Create(shadowMapSkinnedShader);

    shadowMapFBO = FrameBuffer::Create("shadowMap FBO",
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
    auto gWorldPosition = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
    auto gWorldNormal   = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
    auto gWorldTangent  = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });

    auto gAlbedo    = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
    auto gSpecular  = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
    auto gMetallic  = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
    auto gRoughness = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
    auto gViewDepth = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::DEPTH_COMPONENT24, false, WrapMode::ClampToBorder, FilterMode::Nearest });

    // new: extension of gbuffers;
    auto gViewPosition = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
    auto gViewNormal   = Texture2D::CreateEmpty(TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });

    {
        gBufferPassShader = Shader::Create("gBuffer Shader",
                                           "Shaders/Deferred/GBuffer_VS.glsl",
                                           "Shaders/Deferred/GBuffer_FS.glsl");

        Material::SetMaterialProperties(gBufferPassShader);

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
    Material::SetMaterialProperties(litPassShader);

    //=== FBO
    this->litPassFBO = FrameBuffer::Create("litPass FBO",
                                           SCR_WIDTH,
                                           SCR_WIDTH,
                                           FrameBufferType::Regular);

    auto litPassScreenTexture = Texture2D::CreateEmpty(
        TextureSpec { SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F, false, WrapMode::ClampToBorder, FilterMode::Linear, FilterMode::Linear });

    litPassFBO->SetColorTexture(TexType::ScreenTexture, litPassScreenTexture, 0);
    litPassFBO->FinishSetup();

    litOutputs[TexType::ScreenTexture] = litPassScreenTexture;

    //=== material

    auto litPassMaterial = Material::Create(litPassShader);

    litPassMaterial->SetTexture(TexType::gWorldPosition, gWorldPosition);
    litPassMaterial->SetTexture(TexType::gWorldNormal, gWorldNormal);
    litPassMaterial->SetTexture(TexType::gWorldTangent, gWorldTangent);
    litPassMaterial->SetTexture(TexType::gAlbedo, gAlbedo);
    litPassMaterial->SetTexture(TexType::gSpecular, gSpecular);
    litPassMaterial->SetTexture(TexType::gMetallic, gMetallic);
    litPassMaterial->SetTexture(TexType::gRoughness, gRoughness);

    litPassMaterial->SetTexture(TexType::gViewDepth, gViewDepth);

    litPassMaterial->SetTexture(TexType::diffuseEnvMap, diffuseEnvMap);
    litPassMaterial->SetTexture(TexType::specularEnvMap, specularEnvMap);
    litPassMaterial->SetTexture(TexType::brdfLUTTexture, brdfLUTTexture);

    litPassMaterial->SetTexture(TexType::DepthTexture, shadowMap);

    // render quad;
    litPassQuad = ScreenQuad::Create();
    litPassQuad->SetMaterial(litPassMaterial);

    //=====the scene
    // auto scene = Scene::Create();

    // lighting
    this->sunlight = DirectionalLight::Create();
    sunlight->SetIntensity(5.0f);
    sunlight->SetDirection(glm::vec3(0.5f, -0.5f, -1.0f));

    // info for shadowmap:
    // the orthographic projection matrix for the light source：
    sunlight->near_plane      = -10.0f;
    sunlight->far_plane       = 10.0f;
    glm::mat4 lightProjection = glm::ortho(
        -10.0f, 10.0f, -10.0f, 10.0f, sunlight->near_plane, sunlight->far_plane); // the near and far plane should be large enough to cover the scene
    // look at minus direction;
    glm::mat4 lightView = glm::lookAt(-sunlight->m_direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

    sunlight->lightSpaceCamera                     = Camera::Create();
    sunlight->lightSpaceCamera->m_ProjectionMatrix = lightProjection;
    sunlight->lightSpaceCamera->m_ViewMatrix       = lightView;

    //=======actors

    auto floor_gMaterial = PBRMaterial::Create(gBufferPassShader);

    auto floor_albedoMap    = Texture2D::LoadFile("Resources/PBRTextures/Floor_brown_ue/albedo.png");
    auto floor_normalMap    = Texture2D::LoadFile("Resources/PBRTextures/Floor_brown_ue/normal.png");
    auto floor_roughnessMap = Texture2D::LoadFile("Resources/PBRTextures/Floor_brown_ue/roughness.png");
    auto floor_metallicMap  = Texture2D::LoadFile("Resources/PBRTextures/Floor_brown_ue/metallic.png");

    floor_gMaterial->SetTexture(TexType::AlbedoMap, floor_albedoMap);
    floor_gMaterial->SetTexture(TexType::NormalMap, floor_normalMap);
    floor_gMaterial->SetTexture(TexType::RoughnessMap, floor_roughnessMap);
    floor_gMaterial->SetTexture(TexType::MetallicMap, floor_metallicMap);

    auto floor1  = Plane::Create(10);
    floor1->name = "floor1";
    floor1->SetMaterial(floor_gMaterial);
    floor1->transform->scale = glm::vec3(20.0f);
    staticMeshObjects.push_back(floor1);

    if (false)
    {
        auto floor2  = Plane::Create(10);
        floor2->name = "floor2";
        floor2->SetMaterial(floor_gMaterial);

        floor2->transform->scale    = glm::vec3(10.0f);
        floor2->transform->rotation = glm::angleAxis(glm::radians(+90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        floor2->transform->position = glm::vec3(0.0f, 0.0f, -5.0f);
        staticMeshObjects.push_back(floor2);

        auto floor3  = Plane::Create(10);
        floor3->name = "floor3";
        floor3->SetMaterial(floor_gMaterial);

        floor3->transform->scale    = glm::vec3(10.0f);
        floor3->transform->rotation = glm::angleAxis(glm::radians(+90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        floor3->transform->position = glm::vec3(0.0f, -2.0f, -4.0f);
        this->staticMeshObjects.push_back(floor3);
    }

    //
    // color of gold
    glm::vec3 gold_color = glm::vec3(1.0f, 0.766f, 0.336f);

    // draw five spheres each with different roughness
    for (int i = 0; i < 6; i++)
    {
        auto sphere_gMaterial = PBRMaterial::Create(gBufferPassShader);

        sphere_gMaterial->SetVec3("u_Albedo", gold_color);
        sphere_gMaterial->SetFloat("u_Metallic", 1.0f);
        sphere_gMaterial->SetFloat("u_Roughness", (float)i / 5.0f);

        auto sphere  = Sphere::Create(20);
        sphere->name = "sphere" + std::to_string(i);
        sphere->SetMaterial(sphere_gMaterial);
        sphere->transform->position = glm::vec3(-3.0f + i * 2.0f, +1.0f, 0.0f);

        this->staticMeshObjects.push_back(sphere);
    }

    for (int i = 0; i < 6; i++)
    {
        auto sphere_gMaterial = PBRMaterial::Create(gBufferPassShader);

        sphere_gMaterial->SetVec3("u_Albedo", gold_color);
        sphere_gMaterial->SetFloat("u_Metallic", (float)i / 5.0f);
        sphere_gMaterial->SetFloat("u_Roughness", 1.0);

        auto sphere = Sphere::Create(20);
        sphere->SetMaterial(sphere_gMaterial);
        sphere->transform->position = glm::vec3(-3.0f + i * 2.0f, +1.0f, -3.0f);

        staticMeshObjects.push_back(sphere);
    }

    if (false)
    {
        auto sphere_gMaterial = PBRMaterial::Create(gBufferPassShader);

        // sphere_gMaterial->SetVec3("u_Albedo", glm::vec3(1.0f, 0.0f, 0.0f));
        auto sphere_albedoMap    = Texture2D::LoadFile("Resources/PBRTextures/rusted_iron_ue/albedo.png");
        auto sphere_normalMap    = Texture2D::LoadFile("Resources/PBRTextures/rusted_iron_ue/normal.png");
        auto sphere_roughnessMap = Texture2D::LoadFile("Resources/PBRTextures/rusted_iron_ue/roughness.png");
        auto sphere_metallicMap  = Texture2D::LoadFile("Resources/PBRTextures/rusted_iron_ue/metallic.png");

        sphere_gMaterial->SetTexture(TexType::AlbedoMap, sphere_albedoMap);
        sphere_gMaterial->SetTexture(TexType::NormalMap, sphere_normalMap);
        sphere_gMaterial->SetTexture(TexType::RoughnessMap, sphere_roughnessMap);
        sphere_gMaterial->SetTexture(TexType::MetallicMap, sphere_metallicMap);

        auto sphere = Sphere::Create(20);
        sphere->SetMaterial(sphere_gMaterial);
        sphere->transform->position = glm::vec3(1.0f, +1.0f, 0.0f);

        staticMeshObjects.push_back(sphere);
    }

    // model
    // Model::ScaleFactor = 0.01f;
    //  auto test_model = Model::LoadModel("D:/CG_resources/animation/Catwalk Walk Turn 180 Tight.dae");

    if (true)
    {
        // auto test_model = Model::LoadModel("D:/CG_resources/backpack/backpack.obj");
        auto gBufferPassSkinnedShader = Shader::Create("gBuffer Shader",
                                                       "Shaders/Deferred/GBufferSkinned_VS.glsl",
                                                       "Shaders/Deferred/GBuffer_FS.glsl");

        Material::SetMaterialProperties(gBufferPassSkinnedShader);

        // model loading
        Texture2D::SetFlipYOnLoad(true); // eg: for .png;
        Model::s_scaleFactor = 0.01f;

        auto model = Model::LoadModel("Resources/Models/dae/vampire/dancing_vampire.dae");
        this->models.push_back(model);

        // for now: assume model use same shader.
        model->shader = gBufferPassSkinnedShader;

        // set animator
        if (model->hasAnimation())
        {
            auto animator = Animator::Create(model);
            model->SetAnimator(animator);
        }

        for (auto meshObj : model->meshObjects)
        {
            auto _material = meshObj->GetRenderer()->GetMaterial();

            // switch textures to the new material;
            auto new_material = PBRMaterial::Create(gBufferPassSkinnedShader);
            auto textures     = _material->GetTextures();
            new_material->SetTextures(textures);
            meshObj->SetMaterial(new_material);

            _material->SetFloat("u_Metallic", 0.0f);
            _material->SetFloat("u_Roughness", 0.0f);
        }
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
    skyboxShader = Shader::Create("skybox",
                                  "Shaders/Shaders/Skybox_VS.glsl",
                                  "Shaders/Shaders/Skybox_FS.glsl");
    Material::SetMaterialProperties(skyboxShader);

    // material
    auto skyboxMaterial = Material::Create(skyboxShader);
    // skyboxMaterial->SetTexture(TexType::SkyboxTexture, specularEnvMap);
    skyboxMaterial->SetTexture(TexType::SkyboxTexture, envMap);

    skybox = Cube::Create();
    skybox->SetMaterial(skyboxMaterial);

    //
    auto      gridShader = Shader::Create("default Shader", "Shaders/Shaders/default_VS.glsl", "Shaders/Shaders/default_pure_FS.glsl");
    WorldGrid grid       = WorldGrid(20);
    grid.material        = Material::Create(gridShader);

    auto lineShader = Shader::Create("vertex color Shader", "Shaders/Shaders/Vertex_Color_VS.glsl", "Shaders/Shaders/Vertex_Color_FS.glsl");
    this->nav       = new Navigation();
    nav->material   = Material::Create(lineShader);

    screenQuadShader        = Shader::Create("screen quad shader",
                                      "Shaders/Shaders/ScreenQuad_VS.glsl",
                                      "Shaders/Shaders/ScreenQuad_FS.glsl");
    auto screenQuadMaterial = Material::Create(screenQuadShader);

    screenQuad = ScreenQuad::Create();
    screenQuad->SetMaterial(screenQuadMaterial);
}

//=================================================================================================
void PBR::OnUpdate(float deltaTime)
{
    timer += deltaTime;
    // routined updates
    // animations
    {
        for (auto& _model : models)
        {
            if (_model->animator != nullptr)
            {
                _model->animator->UpdateBoneTransforms(timer);
                auto transforms = _model->animator->GetBoneTransforms();
                _model->boneTransformBuffer->SetData(transforms.data(), transforms.size() * sizeof(glm::mat4));
            }
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
            _meshObj->GetRenderer()->Draw(sunlight->lightSpaceCamera, 1, shadowMapMaterial);
        }

        for (auto& _model : models)
        {
            _model->Draw(sunlight->lightSpaceCamera, 1, shadowMapSkinnedMaterial);
        }

        shadowMapFBO->Unbind();
    }

    //======gBuffer pass: render the scene to the gbuffer
    {
        gBufferFBO->Bind();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // make sure clear the framebuffer's content
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glEnable(GL_DEPTH_TEST);

        for (auto& _meshObj : staticMeshObjects)
        {
            _meshObj->Draw(this->GetMainCamera());
        }

        for (auto& _model : models)
        {
            _model->Draw(this->GetMainCamera());
        }

        gBufferFBO->Unbind();
    }

    {
        WorldToViewPass->Render(this->GetMainCamera());
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
        litPassShader->SetVec3("u_CameraPos", this->GetMainCamera()->GetPosition());

        // light info
        litPassShader->SetVec3("u_DirLight.direction", sunlight->m_direction);
        litPassShader->SetVec3("u_DirLight.color", sunlight->m_color);
        litPassShader->SetFloat("u_DirLight.intensity", sunlight->m_intensity);

        litPassShader->SetMat4("u_LightSpaceMatrix", sunlight->lightSpaceCamera->GetProjectionViewMatrix());

        litPassShader->SetBool("u_EnableSkyBox", enableSkyBox);

        litPassShader->SetFloat("u_min_shadow_bias", min_shadow_bias);
        litPassShader->SetFloat("u_max_shadow_bias", max_shadow_bias);

        litPassQuad->Draw(nullptr);

        // glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        litPassFBO->Unbind();
    }

    //=======postprocessing
    if (enableSSAO)
    {
        // RD_PROFILE_SCOPE("SSAO pass");
        SSAOPass->Render(this->GetMainCamera());
    }

    if (enableBloom)
    {
        BloomPass->Render(this->GetMainCamera());
    }

    if (enableOutline)
    {
        OutlinePass->Render(this->GetMainCamera());
    }

    if (enableSSR)
    {
        SSRPass->Render(this->GetMainCamera());
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

    if (enableSkyBox)
    {
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBO->GetFrameBufferID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        skyboxShader->Bind();
        skyboxShader->SetFloat("u_mipLevel", bufferMipLevel);
        skybox->DrawSkybox(this->GetMainCamera());

        skyboxShader->Unbind();
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

        auto                      gbuffer_tex    = gBufferFBO->GetColorTextures();
        std::vector<SharedPtr<Texture>> bufferTextures = {
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
            int channel = g_texture->GetChannels();
            screenQuadShader->SetBool("u_isGrayScale", channel == 1);

            screenQuad->Draw(nullptr);
            index++;
        }
        glEnable(GL_DEPTH_TEST);
    }

    //=======overlays
    // grid.Draw();
    nav->Draw();

    // camera as part of the scene;
    // main_camera->OnUpdate(deltaTime);

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

void PBR::OnImGuiRender()
{
    // 渲染GUI
    ImGui::Begin("Rudy Engine");

    ImGui::Text("Hello World");
    ImGui::Checkbox("enableSkybox", &enableSkyBox);

    std::unordered_map<std::string, SharedPtr<Texture>> bufferList = {
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

    ShowHierarchy();
}

void PBR::ShowTreeNode(const SharedPtr<Transform> transform)
{
    // 如果节点有子节点，使用TreeNodeEx并传递ImGuiTreeNodeFlags_OpenOnArrow
    RD_CORE_ASSERT(transform != nullptr, "transform is null");
    auto name = transform->gameObject->name;

    if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
    {
        // 在这里递归添加子节点
        // ShowTreeNode("Child1");
        // ShowTreeNode("Child2");
        if (transform->children.size() > 0)
        {
            auto children = transform->children;
            for (auto& child : children)
            {
                ShowTreeNode(child);
            }
        }

        ImGui::TreePop(); // 不要忘记结束树节点
    }

    // 处理节点被选中的情况
    if (ImGui::IsItemClicked())
    {
        // 处理选中事件
    }

    // 可选：添加右键菜单
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Rename"))
        {
            // 处理重命名操作
        }
        ImGui::EndPopup();
    }
}

void PBR::ShowHierarchy()
{
    ImGui::Begin("Hierarchy");

    // 为层级中的每个对象调用 ShowTreeNode
    // ShowTreeNode(models[0]->rootNode->transform);
    // ShowTreeNode("Another Node");
    // ... 其他节点 ...

    for (auto& _model : models)
    {
        ShowTreeNode(_model->rootNode->transform);
    }

    for (auto& _meshObj : staticMeshObjects)
    {
        ShowTreeNode(_meshObj->transform);
    }

    ImGui::End();
}

// by render a larger object, and cull front face.
// void HighlightObject()
//{
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_FRONT);
//
//    // pure_color_shader ->Bind();
//    // pure_color_shader->SetBool("u_use_color", true);
//    // pure_color_shader->SetVec3("u_color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
//
//    glDisable(GL_CULL_FACE);
//}

} // namespace Rudy
