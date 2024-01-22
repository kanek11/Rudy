//update 2024.1.22
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


            //if no precomputed envmap available
            auto diffuseEnvMap = TextureCube::LoadHDRI("D:/CG_resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_Env.hdr");

            //lod 5 for specular envmap;
            auto specularEnvMap = envMap->CreatePrefilteredEnvMap(envMap,
                ConvolutionType::Specular, 5);
 

            //BRDF integration map
            auto brdfLUTShader = Shader::Create("brdfLUT Shader", "Shaders/Shaders/BrdfLUT_VS.glsl", "Shaders/Shaders/BrdfLUT_FS.glsl");
            Material::SetMaterialProperties(brdfLUTShader);

            auto brdfQuadMaterial = Material::Create(brdfLUTShader);

            //don't forget to RGBA if ever use as image2D
            auto brdfLUTTexture = Texture2D::CreateEmpty(
                TextureSpec{ 512, 512, TextureInternalFormat::RGB32F });

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
          
            auto gAlbedo  =    Texture2D::CreateEmpty( TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
            auto gSpecular =  Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
            auto gMetallic =  Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
            auto gRoughness = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::R32F });
            auto gViewDepth = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::DEPTH_COMPONENT24,
                                   false, WrapMode::ClampToBorder, FilterMode::Nearest });


            //new: extension of gbuffers;
            auto gViewPosition = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });
            auto gViewNormal =   Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F });



            //set the border color for the depth texture;
            gViewDepth->Bind();
            float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTextureParameterfv(gViewDepth->GetID(), GL_TEXTURE_BORDER_COLOR, borderColor);

            GBufferFBO->SetColorTexture(TexType::gWorldPosition, gWorldPosition, 0);
            GBufferFBO->SetColorTexture(TexType::gAlbedo, gAlbedo, 1);
            GBufferFBO->SetColorTexture(TexType::gWorldNormal, gWorldNormal, 2);
            GBufferFBO->SetColorTexture(TexType::gWorldTangent, gWorldTangent, 3);
            GBufferFBO->SetColorTexture(TexType::gSpecular, gSpecular, 4);
            GBufferFBO->SetColorTexture(TexType::gMetallic, gMetallic, 5);
            GBufferFBO->SetColorTexture(TexType::gRoughness, gRoughness, 6);

            GBufferFBO->SetDepthTexture(gViewDepth);

            GBufferFBO->FinishSetup();  


            //=====the scene 

            //Scene
            auto scene = Scene::Create();

            //lighting
            auto sunlight = DirectionalLight::Create();
            sunlight->intensity = 5.0f;
            sunlight->direction = glm::vec3(0.5f, -0.5f, -1.0f);

            //info for shadowmap:
            //the orthographic projection matrix for the light source：
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


            auto floor3 = Plane::Create(10);
            floor3->transform->scale = glm::vec3(10.0f);
            floor3->transform->rotation = glm::angleAxis(glm::radians(+90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            floor3->transform->position = glm::vec3(0.0f, -2.0f, -4.0f);


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

             
            //model

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
            gViewDepth->Bind();
            float borderColor2[] = { 1.0, 1.0, 1.0, 1.0 };
            glTextureParameterfv(shadowMap->GetID(), GL_TEXTURE_BORDER_COLOR, borderColor2);

            shadowMapFBO->SetDepthTexture(shadowMap);
            shadowMapFBO->FinishSetup();





            //=================================================================================================
            //======lit pass

            //auto litPassShader = Shader::Create("blinnPhong Shader", "Shaders/Shaders/BlinnPhong_VS.glsl", "Shaders/Shaders/BlinnPhong_FS.glsl");
            auto litPassShader = Shader::Create("pbr Shader", "Shaders/Deferred/PBR_VS.glsl", "Shaders/Deferred/PBR_FS.glsl");

            Material::SetMaterialProperties(litPassShader);

            //=== FBO
            litPassFBO = FrameBuffer::Create("litPass FBO",
                SCR_WIDTH, SCR_WIDTH, FrameBufferType::Regular);

            auto litPassScreenTexture = Texture2D::CreateEmpty(
                TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F ,
                false, WrapMode::ClampToBorder, FilterMode::Linear, FilterMode::Linear
                });

 
            litPassFBO->SetColorTexture(TexType::ScreenTexture, litPassScreenTexture, 0);
            litPassFBO->FinishSetup();
          

            litOutputs[TexType::ScreenTexture] = litPassScreenTexture;



            //=== material

            auto litPassMaterial = Material::Create(litPassShader);

            litPassMaterial->SetTexture(TexType::gWorldPosition, gWorldPosition);
            litPassMaterial->SetTexture(TexType::gAlbedo, gAlbedo);
            litPassMaterial->SetTexture(TexType::gWorldNormal, gWorldNormal);
            litPassMaterial->SetTexture(TexType::gWorldTangent, gWorldTangent);
            litPassMaterial->SetTexture(TexType::gSpecular, gSpecular);
            litPassMaterial->SetTexture(TexType::gMetallic, gMetallic);
            litPassMaterial->SetTexture(TexType::gRoughness, gRoughness);
            litPassMaterial->SetTexture(TexType::gViewDepth, gViewDepth);

            litPassMaterial->SetTexture(TexType::diffuseEnvMap, diffuseEnvMap);
            litPassMaterial->SetTexture(TexType::specularEnvMap, specularEnvMap);
            litPassMaterial->SetTexture(TexType::brdfLUTTexture, brdfLUTTexture);

            litPassMaterial->SetTexture(TexType::DepthTexture, shadowMap);


            ////HZ_CORE_ERROR("size of litPass material textures: {0}", litPassMaterial->GetTextures().size());


            //--render quad;
            auto litPassQuad = ScreenQuad::Create();
            litPassQuad->SetMaterial(litPassMaterial);

             
            //=====skybox pass
            auto skyboxShader = Shader::Create("skybox",
                "Shaders/Shaders/Skybox_VS.glsl", "Shaders/Shaders/Skybox_FS.glsl");
            Material::SetMaterialProperties(skyboxShader);

            //material 
            auto skyboxMaterial = Material::Create(skyboxShader);
            skyboxMaterial->SetTexture(TexType::SkyboxTexture, specularEnvMap);
            //skyboxMaterial->SetTexture(TexType::SkyboxTexture,envMap);


            Cube skybox;
            skybox.SetMaterial(skyboxMaterial); 

             


            //world to view
            {
                
                WorldToViewInputs[TexType::gWorldPosition] = gWorldPosition;
                WorldToViewInputs[TexType::gWorldNormal] =   gWorldNormal; 
                WorldToViewInputs[TexType::gViewDepth] = gViewDepth;

                WorldToViewOutputs[TexType::gViewPosition] = gViewPosition;
                WorldToViewOutputs[TexType::gViewNormal] =   gViewNormal;
            
                WorldToViewPass = CreateRef<WorldToView>(SCR_WIDTH, SCR_HEIGHT, WorldToViewInputs, WorldToViewOutputs);
            
            }
  

            // SSAO 
            {

                SSAOInputs[TexType::gViewPosition] = gViewPosition;
                SSAOInputs[TexType::gViewNormal]   = gViewNormal;
                SSAOInputs[TexType::gWorldTangent] = gWorldTangent;
                SSAOInputs[TexType::gViewDepth]  = gViewDepth; 

                SSAOPass = CreateRef<SSAO>(SCR_WIDTH, SCR_HEIGHT, SSAOInputs, SSAOOutputs);
	 
            }


            //Bloom
            {

                BloomInputs[TexType::ScreenTexture] = litPassScreenTexture;

                BloomPass = CreateRef<Bloom>(SCR_WIDTH, SCR_HEIGHT, BloomInputs, BloomOutputs);

            }


            //Outline
            {

                OutlineInputs[TexType::gViewPosition] = gViewPosition;
                OutlineInputs[TexType::gViewNormal] = gViewNormal;

                OutlinePass = CreateRef<Outline>(SCR_WIDTH, SCR_HEIGHT, OutlineInputs, OutlineOutputs);

            }

  
            //SSR
            {

                SSRInputs[TexType::gViewPosition] = gViewPosition;
                SSRInputs[TexType::gViewNormal]   = gViewNormal;
                SSRInputs[TexType::ScreenTexture] = litPassScreenTexture;
                SSRInputs[TexType::gViewDepth] = gViewDepth;

                SSRInputs[TexType::gRoughness] = gRoughness;

                SSRPass =  CreateRef<SSR>(SCR_WIDTH, SCR_HEIGHT, SSRInputs, SSROutputs);
            }
            

            //=================================================================================================

            auto gridShader = Shader::Create("default Shader", "Shaders/Shaders/Default_VS.glsl", "Shaders/Shaders/Default_Flat_FS.glsl");
            WorldGrid grid = WorldGrid(20);
            grid.material = Material::Create(gridShader);


            auto lineShader = Shader::Create("vertex color Shader", "Shaders/Shaders/Vertex_Color_VS.glsl", "Shaders/Shaders/Vertex_Color_FS.glsl");
            Navigation nav = Navigation();
            nav.material = Material::Create(lineShader);


            auto screenQuadShader = Shader::Create("screen quad shader",
                "Shaders/Shaders/ScreenQuad_VS.glsl", "Shaders/Shaders/ScreenQuad_FS.glsl");
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
                 
                //animations
                {
                    animator->UpdateBoneTransforms(timer);
                }

    
                //======gBuffer pass: render the scene to the gbuffer 
                {
                    GBufferFBO->Bind();

                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content  
                    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
                    glEnable(GL_DEPTH_TEST);


                    //get scene camera info 
                    //scene->Render(main_camera);
                    floor1->SetMaterial(plane_gMaterial); 
                    floor1->Draw(main_camera);
                    floor2->SetMaterial(plane_gMaterial);
                    floor2->Draw(main_camera);
                    floor3->SetMaterial(plane_gMaterial);
                    floor3->Draw(main_camera);

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

                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

                    litPassFBO->Bind();

                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
                    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

                    // glDepthMask(GL_FALSE);
                    glDisable(GL_DEPTH_TEST);


                    //scene info
                    litPassShader->Bind();

                    //camera info
                    litPassShader->SetVec3("u_CameraPos", main_camera->GetPosition());

                    //light info
                    litPassShader->SetVec3("u_DirLight.direction", sunlight->direction);
                    litPassShader->SetVec3("u_DirLight.color", sunlight->color);
                    litPassShader->SetFloat("u_DirLight.intensity", sunlight->intensity);

                    litPassShader->SetMat4("u_LightSpaceMatrix", lightSpaceCamera->GetProjectionViewMatrix());

                    //config:
                    litPassShader->SetBool("u_EnableSkyBox", enableSkyBox);

                    litPassQuad->Draw(nullptr);

                    // glDepthMask(GL_TRUE); 
                    glEnable(GL_DEPTH_TEST);


                    litPassFBO->Unbind();


                }


                //=======postprocessing 
                if (enableSSAO)
                {
                    //RD_PROFILE_SCOPE("SSAO pass");
                    SSAOPass->Render(main_camera);
                }

                if (enableBloom)
                {
					BloomPass->Render(main_camera);
				}

                if (enableOutline)
                {
					OutlinePass->Render(main_camera);
                }
                  
                if (enableSSR) 
                {
                    SSRPass->Render(main_camera); 
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
                //visualizeBuffer = BloomOutputs[TexType::ScreenTexture]; 
                //visualizeBuffer = SSAOOutputs[TexType::ScreenTexture];
                if (visualizeBuffer != nullptr)
                {
                    int channel = visualizeBuffer->GetChannels();
                    screenQuadShader->SetBool("u_IsGrayScale", channel == 1);
                    screenQuadShader->SetFloat("u_mipLevel", bufferMipLevel);
                    glBindTextureUnit(0, visualizeBuffer->GetID());


                }

                screenQuad->Draw(nullptr);
                screenQuadShader->Unbind(); 
         

                //=======skybox overlay; on final default framebuffer; 
                //compare the depth with gbuffer;  make sure enable the depth test; 

                if (enableSkyBox)
                {  
                    glEnable(GL_DEPTH_TEST);
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, GBufferFBO->GetFrameBufferID());
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
                   
                    skyboxShader->Bind();
                    skyboxShader->SetFloat("u_mipLevel", bufferMipLevel);
                    skybox.DrawSkybox(main_camera);

                    skyboxShader->Unbind();

                } 


                //=======optional : visualize the buffers；


                if (visualize_gbuffer)
                {
                    //render the buffers to the screen, for debugging 
                    //WARN: don't clear the buffer for overlay;
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

                    std::vector<Ref<Texture>> bufferTextures = {
						 gWorldPosition, 
						 gWorldNormal,
						 gWorldTangent,
                         gAlbedo,
						// gSpecular,
						// gMetallic,
						 gRoughness,
						 gViewDepth,
						 gViewPosition,
						 gViewNormal,
						 //litPassScreenTexture, 
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
                        screenQuadShader->SetBool("u_IsGrayScale", channel == 1);

                        screenQuad->Draw(nullptr);
                        index++;

                    } 
                    glEnable(GL_DEPTH_TEST);

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
        // 初始化ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        // 设置ImGui的样式
        ImGui::StyleColorsDark();
        // 绑定后端
        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)this->window->GetNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 130"); 

        //other settings
        ImGui::SetNextWindowSize(ImVec2(500, 400)); // 设置窗口大小为 500x400

        //initial position on the right top corner;
        ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH - 500, 0)); // 设置窗口位置为 (SCR_WIDTH - 500, 0)
        
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig fontConfig;
        fontConfig.SizePixels = 18.0f; // 设置字体大小为 18 像素
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
        // 开始新的一帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame(); 
        
        //  
        this->DrawGUI();
    }

    void PBR::DrawGUI()
    {
        // 渲染GUI
        ImGui::Begin("Rudy Engine");
        ImGui::Text("Hello World"); 
        ImGui::Checkbox("enableSkybox", &enableSkyBox);


        ImGui::Checkbox("enableSSAO", &enableSSAO);
        ImGui::Checkbox("enableSSAOBlur", &(SSAOPass->enableBlur));
        ImGui::Checkbox("enableBloom", &enableBloom);
        ImGui::Checkbox("enableOutline", &enableOutline);
        ImGui::Checkbox("enableSSR", &enableSSR);


        //group: SSAO

        ImGui::Spacing();
        ImGui::SliderFloat("SSAO radius", &(SSAOPass->radius), 0.0f, 1.0f);
        ImGui::SliderFloat("SSAO bias", &(SSAOPass->bias), 0.0f, 1.0f);
        ImGui::SliderFloat("SSAO range bias", &(SSAOPass->range_bias), 0.0f, 1.0f);
        ImGui::SliderInt ("SSAO kernel size", &(SSAOPass->kernelSize), 0, 64); 

        ImGui::Spacing();
        ImGui::SliderFloat("Bloom threshold", &(BloomPass->threshold), 0.0f, 1.0f);
        ImGui::SliderFloat("Bloom Strength", &(BloomPass->bloom_strength), 0.0f, 3.0f);
        ImGui::SliderFloat("Bloom radius", &(BloomPass->bloom_radius), 0.0f, 1.0f); 

        ImGui::Spacing();
        ImGui::SliderFloat("outline_width", &(OutlinePass->outline_width) ,1.0f  , 5.0f);
        ImGui::SliderFloat("outline depth_thres", &(OutlinePass->depth_thres) , 0.001f , 1.0f);
        ImGui::SliderFloat("outline depth_thres_scale", &(OutlinePass->depth_thres_scale) , 0.001f , 20.0f);
        ImGui::SliderFloat("outline depth_NdotV_threshold", &(OutlinePass->depth_NdotV_threshold) ,0.0f  ,1.0f );
        ImGui::SliderFloat("outline normal_threshold", &(OutlinePass->normal_threshold) , 0.01f , 1.0f);

        ImGui::Checkbox("outline enable depth",  &(OutlinePass->enable_depth));
        ImGui::Checkbox("outline enable normal", &(OutlinePass->enable_normal));


        ImGui::Spacing();
        ImGui::SliderFloat("SSR depth_bias", &(SSRPass->depth_bias), 0.0f, 0.04f);
        ImGui::SliderFloat("SSR step_size", &(SSRPass-> step_size), 0.01f, 0.3f);
        ImGui::SliderInt("SSR max_steps", &(SSRPass->max_steps),  50 , 300);
        ImGui::Checkbox("SSR enableBlur", &(SSRPass->enableBlur));

       
        ImGui::SliderFloat("Buffer miplevel", &bufferMipLevel, 0, 10);

        std::unordered_map<std::string, Ref<Texture>> bufferList = { 
        
         {"Outline output", OutlineOutputs[TexType::ScreenTexture]},
         {"Bloom output",   BloomOutputs[TexType::ScreenTexture]},
         {"SSR output",     SSROutputs[TexType::ScreenTexture]},
         {"SSAO output",    SSAOOutputs[TexType::ScreenTexture]},

         {"Lit Pass output", litOutputs[TexType::ScreenTexture]},

        };

        static int item_current_idx = 0; // 如果你有一个可以从外部访问的默认项，也可以在这里设置它的索引
        std::string item_current = ""; // 用于显示和选择当前项

        if (bufferList.size() > 0) {
            // 创建一个保存所有键的字符串数组
            std::vector<std::string> keys;
            for (const auto& kv : bufferList) {
                keys.push_back(kv.first);
            }

            // 获取当前选中项的键
            item_current = keys[item_current_idx];
            visualizeBuffer = bufferList[item_current];

            if (ImGui::BeginCombo("Visualize Buffer", item_current.c_str())) {
                for (int n = 0; n < keys.size(); n++) {
                    const bool is_selected = (item_current_idx == n);
                    if (ImGui::Selectable(keys[n].c_str(), is_selected)) {
                        item_current_idx = n;
                        visualizeBuffer = bufferList[keys[n]];
                    }

                    // 设置初始焦点项
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            
        } 


        ImGui::End();
    }



    void PBR::RenderGUI()
    { 
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


     

}

