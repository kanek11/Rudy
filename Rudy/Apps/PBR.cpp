//ver 2024.1.17
#include "RudyPCH.h" 
#include "PBR.h"




namespace Rudy
{

     
  
  

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


            main_camera = Camera::Create(MAIN_CAMERA_POS);

            Renderer::Init(SCR_WIDTH, SCR_HEIGHT);
            Renderer::SetMainCamera(main_camera); 
          
            window = Renderer::GetWindow(); 
            auto renderAPI = Renderer::GetAPI();


            //gui
            this->InitGUI();

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
            brdfLUTFBO->SetColorTexture(TexType::brdfLUTTexture, brdfLUTTexture, 0);

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
            auto gWorldPosition = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
            auto gWorldNormal = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
            auto gWorldTangent = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
           
            //new:
            auto gViewPosition = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
            auto gViewNormal = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });

      
            auto gAlbedo = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
            auto gSpecular = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
            auto gMetallic = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
            auto gRoughness = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
            auto gScreenDepth = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::DEPTH_COMPONENT24,
                                   false, WrapMode::ClampToBorder, FilterMode::Nearest });




            //set the border color for the depth texture;
            gScreenDepth->Bind();
            float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTextureParameterfv(gScreenDepth->GetID(), GL_TEXTURE_BORDER_COLOR, borderColor);

            GBufferFBO->SetColorTexture(TexType::gWorldPosition, gWorldPosition, 0);
            GBufferFBO->SetColorTexture(TexType::gAlbedo, gAlbedo, 1);
            GBufferFBO->SetColorTexture(TexType::gWorldNormal, gWorldNormal, 2);
            GBufferFBO->SetColorTexture(TexType::gWorldTangent, gWorldTangent, 3);
            GBufferFBO->SetColorTexture(TexType::gSpecular, gSpecular, 4);
            GBufferFBO->SetColorTexture(TexType::gMetallic, gMetallic, 5);
            GBufferFBO->SetColorTexture(TexType::gRoughness, gRoughness, 6);

            GBufferFBO->SetDepthTexture(gScreenDepth);

            GBufferFBO->FinishSetup();




            //=====the scene 

            //Scene
            auto scene = Scene::Create();

            //lighting
            auto sunlight = DirectionalLight::Create();
            sunlight->intensity = 10.0f;
            sunlight->direction = glm::vec3(0.5f, -0.5f, -1.0f);

            //info for shadowmap:
            //the orthographic projection matrix for the light source��
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


            plane_gMaterial->SetTexture(TexType::AlbedoMap, plane_albedoMap);
            plane_gMaterial->SetTexture(TexType::NormalMap, plane_normalMap);
            plane_gMaterial->SetTexture(TexType::RoughnessMap, plane_roughnessMap);
            plane_gMaterial->SetTexture(TexType::MetallicMap, plane_metallicMap);

             
            auto floor1 = Plane::Create(10);
            floor1->transform->scale = glm::vec3(10.0f);
            floor1->SetMaterial(plane_gMaterial);


            auto floor2 = Plane::Create(10);
            floor2->transform->scale = glm::vec3(10.0f);
            floor2->transform->rotation = glm::angleAxis(glm::radians(+90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
            floor2->transform->position = glm::vec3(0.0f, 0.0f, -5.0f);


            //
            auto sphere_gMaterial = PBRMaterial::Create(gBufferPassShader);

            auto sphere_albedoMap =    Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/albedo.png");
            //sphere_gMaterial->SetVec3("u_Albedo", glm::vec3(1.0f, 0.0f, 0.0f));
            auto sphere_normalMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/normal.png");
            auto sphere_roughnessMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/roughness.png");
            auto sphere_metallicMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/metallic.png");


            sphere_gMaterial->SetTexture(TexType::AlbedoMap,    sphere_albedoMap);
            sphere_gMaterial->SetTexture(TexType::NormalMap, sphere_normalMap);
            sphere_gMaterial->SetTexture(TexType::RoughnessMap, sphere_roughnessMap);
            sphere_gMaterial->SetTexture(TexType::MetallicMap, sphere_metallicMap);


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
            //set the border color for the depth texture;
            gScreenDepth->Bind();
            float borderColor2[] = { 1.0, 1.0, 1.0, 1.0 };
            glTextureParameterfv(shadowMap->GetID(), GL_TEXTURE_BORDER_COLOR, borderColor2);

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
            lightingPassFBO->SetColorTexture(TexType::ScreenTexture, lightingPassScreenTexture, 0);
            lightingPassFBO->FinishSetup();
            lightingPassFBO->Unbind();


            //=== material

            auto lightingPassMaterial = Material::Create(lightingPassShader);

            lightingPassMaterial->SetTexture(TexType::gWorldPosition, gWorldPosition);
            lightingPassMaterial->SetTexture(TexType::gAlbedo, gAlbedo);
            lightingPassMaterial->SetTexture(TexType::gWorldNormal, gWorldNormal);
            lightingPassMaterial->SetTexture(TexType::gWorldTangent, gWorldTangent);
            lightingPassMaterial->SetTexture(TexType::gSpecular, gSpecular);
            lightingPassMaterial->SetTexture(TexType::gMetallic, gMetallic);
            lightingPassMaterial->SetTexture(TexType::gRoughness, gRoughness);
            lightingPassMaterial->SetTexture(TexType::gScreenDepth, gScreenDepth);

            lightingPassMaterial->SetTexture(TexType::diffuseEnvMap, diffuseEnvMap);
            lightingPassMaterial->SetTexture(TexType::specularEnvMap, specularEnvMap);
            lightingPassMaterial->SetTexture(TexType::brdfLUTTexture, brdfLUTTexture);

            lightingPassMaterial->SetTexture(TexType::DepthTexture, shadowMap);


            ////HZ_CORE_ERROR("size of lightingpass material textures: {0}", lightingPassMaterial->GetTextures().size());


            //--render quad;
            auto lightingPassQuad = ScreenQuad::Create();
            lightingPassQuad->SetMaterial(lightingPassMaterial);




            //=====skybox pass
            auto skyboxShader = Shader::Create("skybox", "Shaders/Shaders/Skybox_VS.glsl", "Shaders/Shaders/Skybox_FS.glsl");
            Material::SetMaterialProperties(skyboxShader);

            //material 
            auto skyboxMaterial = Material::Create(skyboxShader);
            skyboxMaterial->SetTexture(TexType::SkyboxTexture, envMap);

            Cube skybox;
            skybox.SetMaterial(skyboxMaterial);






            //world to view
            {
                
                WorldToViewInputs[TexType::gWorldPosition] = gWorldPosition;
                WorldToViewInputs[TexType::gWorldNormal] = gWorldNormal;

                WorldToViewOutputs[TexType::gViewPosition] = gViewPosition;
                WorldToViewOutputs[TexType::gViewNormal] = gViewNormal;
            
                WorldToViewPass = CreateRef<WorldToView>(SCR_WIDTH, SCR_HEIGHT, WorldToViewInputs, WorldToViewOutputs);
            
            }



             

            // SSAO pass;
            {

                SSAOInputs[TexType::gViewPosition] = gViewPosition;
                SSAOInputs[TexType::gViewNormal]   = gViewNormal;
                SSAOInputs[TexType::gWorldTangent] = gWorldTangent;
                SSAOInputs[TexType::gScreenDepth]  = gScreenDepth; 

                SSAOPass = CreateRef<SSAO>(SCR_WIDTH, SCR_HEIGHT, SSAOInputs, SSAOOutputs);
	 


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
                ssrMaterial->SetTexture(TexType::gWorldPosition, gWorldPosition);
                ssrMaterial->SetTexture(TexType::gWorldNormal, gWorldNormal);
                ssrMaterial->SetTexture(TexType::gScreenDepth, gScreenDepth);
                ssrMaterial->SetTexture(TexType::lightingPassTexture, lightingPassScreenTexture);


                //auto ssrQuad = Quad();
                ssrQuad->SetMaterial(ssrMaterial);

                //auto ssrScreenTexture = Texture2D::CreateEmpty(
                //    TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F,
                //                 false, WrapMode::ClampToBorder, FilterMode::Linear });

                ssrFBO->SetColorTexture(TexType::ScreenTexture, ssrScreenTexture, 0);
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

                //RD_PROFILE_SCOPE("the game loop");

                //get the time of each frame
                float time = (float)glfwGetTime();
                float deltaTime = time - lastFrameTime;
                lastFrameTime = time;
                timer += deltaTime;


                //'gui'
                this->PrepareGUI();

            

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
                    floor1->SetMaterial(plane_gMaterial); 
                    floor1->Draw(main_camera);
                    floor2->SetMaterial(plane_gMaterial);
                    floor2->Draw(main_camera);

                    sphere->SetMaterial(sphere_gMaterial);
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




                { 
                   WorldToViewPass->Render(main_camera);
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
                    floor1->SetMaterial(shadowMapMaterial);
                    floor1->Draw(lightSpaceCamera);


                    sphere->SetMaterial(shadowMapMaterial);  
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
                    //RD_PROFILE_SCOPE("SSAO pass");
                    SSAOPass->Render(main_camera);
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

                

                //===========debug: visualize any texture; 
                //todo: make a good filter for ss pass;  
                // add a "composer"  or design the blend mode;

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDisable(GL_DEPTH_TEST); 
                glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

                screenQuadShader->Bind();


                //  glEnable(GL_BLEND);
                //  glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
                //
                screenQuadShader->SetBool("u_IsGrayScale", true);
                glBindTextureUnit(0, SSAOOutputs[TexType::ScreenTexture]->GetID());  //replace the texture2D here;
                

              //screenQuadShader->SetBool("u_IsGrayScale", false);
              //glBindTextureUnit(0, WorldToViewOutputs[TexType::gViewPosition]->GetID());  //replace the texture2D here;

                 
               //screenQuadShader->SetBool("u_IsGrayScale", false);
               //glBindTextureUnit(0, WorldToViewInputs[TexType::gWorldNormal]->GetID());  //replace the texture2D here;


                //screenQuadShader->SetBool("u_IsGrayScale", false);
                //glBindTextureUnit(0, lightingPassScreenTexture->GetID());  //replace the texture2D here;

                screenQuad->Draw(nullptr);
            

              



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





                //=======optional : visualize the buffers��


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
                        glBindTextureUnit(0, g_texture.second->GetID());
                        if (g_texture.first == TexType::gMetallic || g_texture.first == TexType::gRoughness || g_texture.first == TexType::gSpecular)
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



                //======gui
                this->RenderGUI();
                Renderer::WindowOnUpdate();
                /* Swap front and back buffers */
               // glfwSwapBuffers(window); 
                /* Poll for and process events */
                //glfwPollEvents();



            }


            //====shutdown
            this->ShutDownGUI();
            glfwTerminate(); 


    }











    void PBR::InitGUI()
    {
        // ��ʼ��ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        // ����ImGui����ʽ
        ImGui::StyleColorsDark();
        // �󶨺��
        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)this->window->GetNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 130");


        //other settings
        ImGui::SetNextWindowSize(ImVec2(500, 400)); // ���ô��ڴ�СΪ 500x400

        //initial position on the right top corner;
        //ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH - 500, 0)); // ���ô���λ��Ϊ (SCR_WIDTH - 500, 0)
        

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig fontConfig;
        fontConfig.SizePixels = 18.0f; // ���������СΪ 18 ����
        io.Fonts->AddFontDefault(&fontConfig);


    }

    void PBR::ShutDownGUI()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void PBR::PrepareGUI()
    {
        // ��ʼ�µ�һ֡
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

     
        
        //  
        this->DrawGUI();
    }

    void PBR::DrawGUI()
    {
        // ��ȾGUI
        ImGui::Begin("Rudy Engine");
        ImGui::Text("Hello World"); 


        ImGui::Checkbox("enableSSAO", &enableSSAO);
        ImGui::Checkbox("enableSSAOBlur", &(SSAOPass->enableBlur));


        ImGui::SliderFloat("SSAO radius", &(SSAOPass->radius), 0.0f, 1.0f);
        ImGui::SliderFloat("SSAO bias", &(SSAOPass->bias), 0.0f, 1.0f);
       ImGui::SliderFloat("SSAO range bias", &(SSAOPass->range_bias), 0.0f, 1.0f);


        ImGui::SliderInt ("SSAO kernel size", &(SSAOPass->kernelSize), 0, 64);




        ImGui::End();
    }

    void PBR::RenderGUI()
    {
        // ��ȾGUI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }




















}

