//ver 2024.1.17
#include "RudyPCH.h" 
#include "PBR.h"




namespace Rudy
{

     
    //2560:1440 = 16:9
    const uint32_t SCR_WIDTH = 2560;
    const uint32_t SCR_HEIGHT = 1440;
    const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
    const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
    const uint32_t SHADOW_WIDTH = 2560, SHADOW_HEIGHT = 2560;


    const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 1.5f, 5.0f);


    PBR::PBR() : Application()
    {
		//this->GetWindow()->SetVSync(false);
	}

    Ref<PBR> PBR::Create()
    {
		return CreateRef<PBR>();
	}


    void PBR::Init()
    {

     }
  
    void PBR::Start()
    { 

            Rudy::Log::Init();
            RD_CORE_WARN("test:Initialized Log!");

            //========================================
            //=== initialize renderer
            //auto window = Window::Create();
            //Input::SetWindowContext(window->GetNativeWindow());


            auto main_camera = Camera::Create(MAIN_CAMERA_POS);

            Renderer::Init(SCR_WIDTH, SCR_HEIGHT);
            Renderer::SetMainCamera(main_camera);

            auto renderAPI = Renderer::s_RendererAPI;



            //=================================================================================================
            //=== initialize the resources


            //=======environment map precomputing pass

            Texture2D::SetFlipYOnLoad(true);
            // auto testHDRI = Texture2D::LoadFile("D:/CG_resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr");

            auto envMap = TextureCube::LoadHDRI("D:/CG_resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr");

            auto diffuseEnvMap = TextureCube::LoadHDRI("D:/CG_resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_Env.hdr");

            //lod 5 for specular envmap;
            auto specularEnvMap = envMap->CreatePrefilteredEnvMap(envMap,
                ConvolutionType::Specular, 5);

            //if no precomputed envmap available
            //auto diffuseEnvMap = envMap->CreatePrefilteredEnvMap(envMap,
            //    ConvolutionType::Diffuse); 



            //the LUT for BRDF
            auto brdfLUTShader = Shader::Create("brdfLUT Shader", "Shaders/Shaders/BrdfLUT_VS.glsl", "Shaders/Shaders/BrdfLUT_FS.glsl");
            Material::SetMaterialProperties(brdfLUTShader);

            auto brdfQuadMaterial = Material::Create(brdfLUTShader);

            auto brdfLUTTexture = Texture2D::CreateEmpty(TextureSpec{ 512, 512, TextureInternalFormat::RGB32F });

            auto brdfLUTFBO = FrameBuffer::Create("brdfLUT FBO", 512, 512, FrameBufferType::Regular);

            brdfLUTFBO->Bind();
            brdfLUTFBO->SetColorTexture(TextureType::brdfLUTTexture, brdfLUTTexture, 0);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            glViewport(0, 0, 512, 512);

            auto brdfQuad = ScreenQuad::Create();
            brdfQuad->SetMaterial(brdfQuadMaterial);
            brdfQuad->Draw(nullptr);
            brdfLUTFBO->Unbind();


            //test for separate  
           /*
           Texture2D::SetFlipYOnLoad(false);
           std::string cubeMapPath = "D:/CG_resources/skybox/";
           std::vector<std::string> paths =
           {
              cubeMapPath + "right.jpg",
              cubeMapPath + "left.jpg",
              cubeMapPath + "top.jpg",
              cubeMapPath + "bottom.jpg",
              cubeMapPath + "front.jpg",
              cubeMapPath + "back.jpg"
           };

           auto test_envCubeMap = TextureCube::LoadImages(paths);
            */




            //======gbuffer pass;

            auto gBufferPassShader = Shader::Create("gBuffer Shader",
                "Shaders/Deferred/GBuffer_VS.glsl", "Shaders/Deferred/GBuffer_FS.glsl");

            Material::SetMaterialProperties(gBufferPassShader);

            //Framebuffer;
            auto GBufferFBO = FrameBuffer::Create(
                "GBuffer FBO", SCR_WIDTH, SCR_WIDTH, FrameBufferType::GBuffer);

            //the framebuffer for gbuffer pass: 8;
            auto gPosition = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F });
            auto gAlbedo = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F });
            auto gWorldNormal = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F });
            auto gWorldTangent = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F });
            auto gSpecular = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
            auto gMetallic = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
            auto gRoughness = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
            auto gScreenDepth = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::DEPTH_COMPONENT24,
                                     false, WrapMode::ClampToBorder, FilterMode::Nearest });


            GBufferFBO->SetColorTexture(TextureType::gPosition, gPosition, 0);
            GBufferFBO->SetColorTexture(TextureType::gAlbedo, gAlbedo, 1);
            GBufferFBO->SetColorTexture(TextureType::gWorldNormal, gWorldNormal, 2);
            GBufferFBO->SetColorTexture(TextureType::gWorldTangent, gWorldTangent, 3);
            GBufferFBO->SetColorTexture(TextureType::gSpecular, gSpecular, 4);
            GBufferFBO->SetColorTexture(TextureType::gMetallic, gMetallic, 5);
            GBufferFBO->SetColorTexture(TextureType::gRoughness, gRoughness, 6);

            GBufferFBO->SetDepthTexture(gScreenDepth);

            GBufferFBO->FinishSetup();




            //=====the scene 

            //Scene
            auto scene = Scene::Create();

            //lighting
            auto sunlight = DirectionalLight::Create();
            sunlight->direction = glm::vec3(0.5f, -0.5f, -1.0f);

            //info for shadowmap:
            //the orthographic projection matrix for the light source£º
            glm::mat4 lightProjection = glm::ortho(
                -20.0f, 20.0f, -20.0f, 20.0f, -10.0f, 20.0f);  //the near and far plane should be large enough to cover the scene
            //look at minus direction;
            glm::mat4 lightView = glm::lookAt(sunlight->direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

            auto lightSpaceCamera = Camera::Create();
            lightSpaceCamera->m_ProjectionMatrix = lightProjection;
            lightSpaceCamera->m_ViewMatrix = lightView;



            //plane;
            auto plane_gMaterial = PBRMaterial::Create(gBufferPassShader);

            auto plane_albedoMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/albedo.png");
            auto plane_normalMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/normal.png");
            auto plane_roughnessMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/roughness.png");
            auto plane_metallicMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/metallic.png");


            plane_gMaterial->SetTexture(TextureType::AlbedoMap, plane_albedoMap);
            plane_gMaterial->SetTexture(TextureType::NormalMap, plane_normalMap);
            plane_gMaterial->SetTexture(TextureType::RoughnessMap, plane_roughnessMap);
            plane_gMaterial->SetTexture(TextureType::MetallicMap, plane_metallicMap);




            auto floor = Plane::Create(30);
            //floor->transform->position = glm::vec3(0.0f, -1.0f, 0.0f);
            floor->SetMaterial(plane_gMaterial);


            //
            auto sphere_gMaterial = PBRMaterial::Create(gBufferPassShader);

            auto sphere_albedoMap =    Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/albedo.png");
            //sphere_gMaterial->SetVec3("u_Albedo", glm::vec3(1.0f, 0.0f, 0.0f));
            auto sphere_normalMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/normal.png");
            auto sphere_roughnessMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/roughness.png");
            auto sphere_metallicMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/metallic.png");


            //sphere_gMaterial->SetTexture(TextureType::AlbedoMap,    sphere_albedoMap);
            sphere_gMaterial->SetTexture(TextureType::NormalMap, sphere_normalMap);
            sphere_gMaterial->SetTexture(TextureType::RoughnessMap, sphere_roughnessMap);
            sphere_gMaterial->SetTexture(TextureType::MetallicMap, sphere_metallicMap);


            auto sphere = Sphere::Create(20);
            sphere->SetMaterial(sphere_gMaterial);

            sphere->transform->position = glm::vec3(1.0f, +1.0f, 0.0f);




            //==========shadow map pass
            auto shadowMapShader = Shader::Create("shadow map Shader",
                "Shaders/Shaders/DepthMap_VS.glsl",
                "Shaders/Shaders/DepthMap_FS.glsl");
            Material::SetMaterialProperties(shadowMapShader);

            auto shadowMapSkinnedShader = Shader::Create("shadow map Shader",
                "Shaders/Shaders/DepthMapSkinned_VS.glsl",
                "Shaders/Shaders/DepthMap_FS.glsl");
            Material::SetMaterialProperties(shadowMapSkinnedShader);


            auto shadowMapMaterial = Material::Create(shadowMapShader);
            auto shadowMapSkinnedMaterial = Material::Create(shadowMapSkinnedShader);


            auto shadowMapFBO = FrameBuffer::Create("shadowMap FBO",
                SHADOW_WIDTH, SHADOW_HEIGHT, FrameBufferType::DepthTexture);

            auto shadowMap = Texture2D::CreateEmpty(
                TextureSpec{ SHADOW_WIDTH, SHADOW_HEIGHT, TextureInternalFormat::DEPTH_COMPONENT24,
                             false, WrapMode::ClampToBorder, FilterMode::Nearest });


            shadowMapFBO->SetDepthTexture(shadowMap);
            shadowMapFBO->FinishSetup();


            //     
            //===========new: animated model

             //auto test_model = Model::LoadModel("D:/CG_resources/backpack/backpack.obj");
            auto gBufferPassSkinnedShader = Shader::Create("gBuffer Shader",
                "Shaders/Deferred/GBufferSkinned_VS.glsl",
                "Shaders/Deferred/GBuffer_FS.glsl");

            Material::SetMaterialProperties(gBufferPassSkinnedShader);


            Texture2D::SetFlipYOnLoad(true); //eg: for .png;

            Model::s_scaleFactor = 0.01f;
            auto test_model = Model::LoadModel("D:/CG_resources/dae/vampire/dancing_vampire.dae");


            std::vector< Ref<Material> > model_materials;


            for (auto meshObj : test_model->meshObjects)
            {
                auto _material = meshObj->GetRendererComponent()->GetMaterial();
                _material->SetShader(gBufferPassSkinnedShader);
                model_materials.push_back(_material);

                _material->SetFloat("u_Metallic", 0.0f);
                _material->SetFloat("u_Roughness", 0.0f);
            }

            //Model::ScaleFactor = 0.01f;
           // auto test_model = Model::LoadModel("D:/CG_resources/animation/Catwalk Walk Turn 180 Tight.dae"); 

            //test: update the global transform of the model; according to the transform of bones;
            //Transform::UpdateWorldTransformRecursive(test_model->rootNode->transform, glm::mat4(1.0f)); 



            auto transforms = std::vector<glm::mat4>(100, glm::mat4(1.0f));

            //test if the animation works;
            auto test_animation_clip = test_model->animationClip;
            auto animator = Animator::Create();
            if (test_animation_clip != nullptr)
            {

                //animation_clip->CalculateKeyframe(0);
              //auto transform = animation_clip->GetGlobalTransform("Hips");

        //pose it to the first frame; 
                animator->model = test_model;
                animator->animationClip = test_model->animationClip;

                animator->UpdateBoneTransforms(0.0f);
                transforms = animator->GetBoneTransforms();

            }


            gBufferPassSkinnedShader->Bind();

            //RD_CORE_INFO("get transforms of size{0}", transforms.size());
            for (int i = 0; i < 100; ++i)
                gBufferPassSkinnedShader->SetMat4("u_BoneTransforms[" + std::to_string(i) + "]", transforms[i]);
            //  gBufferPassSkinnedShader->SetMat4("u_BoneTransforms[" + std::to_string(i) + "]", glm::mat4(1.0f));
            gBufferPassSkinnedShader->SetMat4("u_TestIdentity", glm::mat4(1.0f));


            gBufferPassSkinnedShader->Unbind();






            //=================================================================================================
            //======lighting pass

            //auto lightingPassShader = Shader::Create("blinnPhong Shader", "Shaders/Shaders/BlinnPhong_VS.glsl", "Shaders/Shaders/BlinnPhong_FS.glsl");
            auto lightingPassShader = Shader::Create("pbr Shader", "Shaders/Deferred/PBR_VS.glsl", "Shaders/Deferred/PBR_FS.glsl");

            Material::SetMaterialProperties(lightingPassShader);

            //=== FBO
            auto lightingPassFBO = FrameBuffer::Create("lightingPass FBO",
                SCR_WIDTH, SCR_WIDTH, FrameBufferType::Regular);

            auto lightingPassScreenTexture = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F });

            lightingPassFBO->Bind();
            lightingPassFBO->SetColorTexture(TextureType::ScreenTexture, lightingPassScreenTexture, 0);
            lightingPassFBO->FinishSetup();
            lightingPassFBO->Unbind();


            //=== material

            auto lightingPassMaterial = Material::Create(lightingPassShader);

            lightingPassMaterial->SetTexture(TextureType::gPosition, gPosition);
            lightingPassMaterial->SetTexture(TextureType::gAlbedo, gAlbedo);
            lightingPassMaterial->SetTexture(TextureType::gWorldNormal, gWorldNormal);
            lightingPassMaterial->SetTexture(TextureType::gWorldTangent, gWorldTangent);
            lightingPassMaterial->SetTexture(TextureType::gSpecular, gSpecular);
            lightingPassMaterial->SetTexture(TextureType::gMetallic, gMetallic);
            lightingPassMaterial->SetTexture(TextureType::gRoughness, gRoughness);
            lightingPassMaterial->SetTexture(TextureType::gScreenDepth, gScreenDepth);

            lightingPassMaterial->SetTexture(TextureType::diffuseEnvMap, diffuseEnvMap);
            lightingPassMaterial->SetTexture(TextureType::specularEnvMap, specularEnvMap);
            lightingPassMaterial->SetTexture(TextureType::brdfLUTTexture, brdfLUTTexture);

            lightingPassMaterial->SetTexture(TextureType::DepthTexture, shadowMap);


            ////HZ_CORE_ERROR("size of lightingpass material textures: {0}", lightingPassMaterial->GetTextures().size());


            //--render quad;
            auto lightingPassQuad = ScreenQuad::Create();
            lightingPassQuad->SetMaterial(lightingPassMaterial);




            //=====skybox pass
            auto skyboxShader = Shader::Create("skybox", "Shaders/Shaders/Skybox_VS.glsl", "Shaders/Shaders/Skybox_FS.glsl");
            Material::SetMaterialProperties(skyboxShader);

            //material 
            auto skyboxMaterial = Material::Create(skyboxShader);
            skyboxMaterial->SetTexture(TextureType::SkyboxTexture, envMap);

            Cube skybox;
            skybox.SetMaterial(skyboxMaterial);




            //========postprocess: SSAO pass;
            {

                ssaoInputs[TextureType::gPosition] = gPosition;
                ssaoInputs[TextureType::gWorldNormal] = gWorldNormal;
                ssaoInputs[TextureType::gWorldTangent] = gWorldTangent;
                ssaoInputs[TextureType::gScreenDepth] = gScreenDepth;


                auto ssaoScreenTexture = Texture2D::CreateEmpty(
                    TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F,
                                 false, WrapMode::ClampToBorder, FilterMode::Nearest });

               ssaoOutputs[TextureType::ScreenTexture] = ssaoScreenTexture;

               ssaoPass = SSAO::Create(SCR_WIDTH, SCR_HEIGHT, ssaoInputs, ssaoOutputs);
	 


            }

            //=================================================================================================



            //==========SSR; 


            auto ssrShader = Shader::Create("ssr Shader", "Shaders/PostProcess/SSR_VS.glsl", "Shaders/PostProcess/SSR_FS.glsl");
            Material::SetMaterialProperties(ssrShader);

            auto ssrFBO = FrameBuffer::Create(" ssr FBO",
                SCR_WIDTH, SCR_WIDTH, FrameBufferType::Regular);

            auto ssrQuad = ScreenQuad::Create();

            auto ssrScreenTexture = Texture2D::CreateEmpty(
                TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F,
                           false, WrapMode::ClampToBorder, FilterMode::Linear });


            if (enableSSR)
            {

                ssrShader->Bind();
                ssrShader->SetMat4("u_ProjectionView", main_camera->GetProjectionViewMatrix());
                ssrShader->SetVec3("u_CameraPos", main_camera->GetPosition());

                auto ssrMaterial = Material::Create(ssrShader);
                ssrMaterial->SetTexture(TextureType::gPosition, gPosition);
                ssrMaterial->SetTexture(TextureType::gWorldNormal, gWorldNormal);
                ssrMaterial->SetTexture(TextureType::gScreenDepth, gScreenDepth);
                ssrMaterial->SetTexture(TextureType::lightingPassTexture, lightingPassScreenTexture);


                //auto ssrQuad = Quad();
                ssrQuad->SetMaterial(ssrMaterial);

                //auto ssrScreenTexture = Texture2D::CreateEmpty(
                //    TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F,
                //                 false, WrapMode::ClampToBorder, FilterMode::Linear });

                ssrFBO->SetColorTexture(TextureType::ScreenTexture, ssrScreenTexture, 0);
                ssrFBO->FinishSetup();

            }




            //=================================================================================================

            auto defaultShader = Shader::Create("default Shader", "Shaders/Shaders/Default_VS.glsl", "Shaders/Shaders/Default_Flat_FS.glsl");
            WorldGrid grid = WorldGrid(20);
            grid.material = Material::Create(defaultShader);


            auto lineShader = Shader::Create("vertex color Shader", "Shaders/Shaders/Vertex_Color_VS.glsl", "Shaders/Shaders/Vertex_Color_FS.glsl");
            Navigation nav = Navigation();
            nav.material = Material::Create(lineShader);


            auto screenQuadShader = Shader::Create("screen quad shader", "Shaders/Shaders/ScreenQuad_VS.glsl", "Shaders/Shaders/ScreenQuad_FS.glsl");
            auto screenQuadMaterial = Material::Create(screenQuadShader);

            auto screenQuad = ScreenQuad::Create();
            screenQuad->SetMaterial(screenQuadMaterial);


            //======the loop 
            /* Loop until the user closes the window */


            float lastFrameTime = 0.0f;
            float timer = 0.0f;
            RD_CORE_WARN("App: Entering the loop");
            while (!Renderer::ShouldClose())
            {
                //get the time of each frame
                float time = (float)glfwGetTime();
                float deltaTime = time - lastFrameTime;
                lastFrameTime = time;
                timer += deltaTime;

                {
                    animator->UpdateBoneTransforms(timer);
                }

                /* Render here */
                //glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
                //glClear(GL_COLOR_BUFFER_BIT);


                //======g buffer pass: render the scene to the gbuffer 
                {
                    GBufferFBO->Bind();

                    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content  
                    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
                    glEnable(GL_DEPTH_TEST);


                    //get scene camera info 
                    //scene->Render(main_camera);
                    floor->SetMaterial(plane_gMaterial);
                    sphere->SetMaterial(sphere_gMaterial);
                    floor->Draw(main_camera);
                    sphere->Draw(main_camera);

                    int index = 0;
                    for (auto meshObj : test_model->meshObjects)
                    {
                        meshObj->SetMaterial(model_materials[index++]);
                    }

                    gBufferPassSkinnedShader->Bind();
                    auto transforms = animator->GetBoneTransforms();
                    for (int i = 0; i < 100; ++i)
                        gBufferPassSkinnedShader->SetMat4("u_BoneTransforms[" + std::to_string(i) + "]", transforms[i]);

                    gBufferPassSkinnedShader->Unbind();

                    test_model->Draw(main_camera);


                    GBufferFBO->Unbind();
                }


                //====== shadowMap pass: render the scene to the shadowMap;
                if (false)
                {
                    shadowMapFBO->Bind();

                    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                    glClear(GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content 

                    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

                    glEnable(GL_DEPTH_TEST);

                    //render the scene to the shadowMap; 
                    floor->SetMaterial(shadowMapMaterial);
                    sphere->SetMaterial(shadowMapMaterial);

                    floor->Draw(lightSpaceCamera);
                    sphere->Draw(lightSpaceCamera);


                    //test_model->SetShader(shadowMapSkinnedShader );
                    //shadowMapSkinnedShader->Bind();
                    // 
                    //auto transforms = animator->GetBoneTransforms();
                    //for (int i = 0; i < 100; ++i)
                    //    shadowMapSkinnedShader->SetMat4("u_BoneTransforms[" + std::to_string(i) + "]", transforms[i]);
                    //
                    //shadowMapSkinnedShader->Unbind();
                    //test_model->Draw(lightSpaceCamera);  //todo:  skinned mesh need special treatment for shadowmap too;


                    shadowMapFBO->Unbind();

                }


                //======= lighting pass: render the quad;
                if (true)
                {

                    lightingPassFBO->Bind();

                    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

                    // glDepthMask(GL_FALSE);
                    glDisable(GL_DEPTH_TEST);


                    //scene info
                    lightingPassShader->Bind();

                    //camera info
                    lightingPassShader->SetVec3("u_CameraPos", main_camera->GetPosition());

                    //light info
                    lightingPassShader->SetVec3("u_DirLight.direction", sunlight->direction);
                    lightingPassShader->SetVec3("u_DirLight.color", sunlight->color);
                    lightingPassShader->SetFloat("u_DirLight.intensity", sunlight->intensity);

                    lightingPassShader->SetMat4("u_LightSpaceMatrix", lightSpaceCamera->GetProjectionViewMatrix());

                    //config:
                    lightingPassShader->SetBool("u_EnableSkyBox", enableSkyBox);

                    lightingPassQuad->Draw(nullptr);

                    // glDepthMask(GL_TRUE); 
                    glEnable(GL_DEPTH_TEST);


                    lightingPassFBO->Unbind();


                }


                //=======postprocessing

                //=====SSAO
                if (enableSSAO)
                {
                    ssaoPass->Render(main_camera);
                }



                //=====SSR
                if (enableSSR)

                {
                    ssrFBO->Bind();
                    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glDisable(GL_DEPTH_TEST);
                    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

                    ssrShader->Bind();
                    ssrShader->SetMat4("u_ProjectionView", main_camera->GetProjectionViewMatrix());
                    ssrShader->SetMat4("u_View", main_camera->GetViewMatrix());
                    ssrShader->SetMat4("u_Projection", main_camera->GetProjectionMatrix());
                    ssrShader->SetVec3("u_CameraPos", main_camera->GetPosition());
                    //HZ_CORE_INFO("cameraPos: {0}", main_camera->GetPosition());

                    ssrQuad->Draw(nullptr);
                    ssrFBO->Unbind();

                }






                //==========on default framebuffer;

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                //===========debug: visualize any texture; 
                //todo: make a good filter for ss pass;  
                // add a "composer"  or design the blend mode;

                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDisable(GL_DEPTH_TEST);

                glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
                screenQuadShader->Bind();


                //  glEnable(GL_BLEND);
                //  glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
                //
                screenQuadShader->SetBool("u_IsGrayScale", true);
                glBindTextureUnit(0, ssaoOutputs[TextureType::ScreenTexture]->GetTextureID());  //replace the texture2D here;
                screenQuad->Draw(nullptr);

                  //screenQuadShader->SetBool("u_IsGrayScale", true);
                  //glBindTextureUnit(0, gAlbedo->GetTextureID());  //replace the texture 2D here; 
               //glBindTextureUnit(0, lightingPassScreenTexture->GetTextureID());  //replace the texture 2D here; 
              

                //screenQuadShader->SetBool("u_IsGrayScale", true);
                //glBindTextureUnit(0, ssrScreenTexture->GetTextureID());  //replace the texture2D here;



                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);

                //=======skybox overlay; on final default framebuffer; 
                //compare the depth with gbuffer;  make sure enable the depth test; 

                if (enableSkyBox)
                {

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    glEnable(GL_DEPTH_TEST);
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, GBufferFBO->GetFrameBufferID());
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
                    skybox.DrawSkybox(main_camera);


                }





                //=======optional : visualize the buffers£»


                if (visualize_buffer)
                {
                    //render the buffers to the screen, for debugging
            // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
               //WARN: don't clear the buffer if you want overlay;
                    glDisable(GL_DEPTH_TEST);

                    std::vector<std::pair<int, int>> leftBottom =
                    { {0, 0},
                      {1, 0},
                      {2, 0},
                      {3, 0},
                      {0, 3},
                      {1, 3},
                      {2, 3},
                      {3, 3}, };


                    auto bufferTextures = lightingPassMaterial->GetTextures();

                    int index = 0;
                    for (auto& g_texture : bufferTextures)

                    {
                        if (index >= 8)
                            break;
                        screenQuadShader->Bind();

                        glViewport(leftBottom[index].first * BUFFER_WIDTH, leftBottom[index].second * BUFFER_HEIGHT, BUFFER_WIDTH, BUFFER_HEIGHT);
                        glBindTextureUnit(0, g_texture.second->GetTextureID());
                        if (g_texture.first == TextureType::gMetallic || g_texture.first == TextureType::gRoughness || g_texture.first == TextureType::gSpecular)
                        {
                            screenQuadShader->SetBool("u_IsGrayScale", true);
                            // HZ_CORE_WARN("gray scale");
                        }
                        else
                        {
                            screenQuadShader->SetBool("u_IsGrayScale", false);
                        }

                        screenQuad->Draw(nullptr);
                        index++;

                    }

                    glDisable(GL_DEPTH_TEST);

                }


                //=======overlay: world grid; 
                // grid.Draw();  
                nav.Draw();


                main_camera->OnUpdate(deltaTime);

                Renderer::WindowOnUpdate();
                /* Swap front and back buffers */
               // glfwSwapBuffers(window); 
                /* Poll for and process events */
                //glfwPollEvents();



            }


            //====shutdown

            glfwTerminate(); 


    }

}

