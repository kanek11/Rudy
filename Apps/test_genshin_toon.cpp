//ver 2023.12.30
#include "RudyPCH.h" 
#include <Rudy.h>





void Start() {

    bool  visualize_buffer = false;
    bool  enableSkyBox = true;
    bool  enableSSAO = false;
    bool  enableSSR = false;


    //2560:1440 = 16:9
    const uint32_t SCR_WIDTH = 2560;
    const uint32_t SCR_HEIGHT = 1440;
    const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
    const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
    const uint32_t SHADOW_WIDTH = 2560, SHADOW_HEIGHT = 2560;


    const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 2.0f, 2.0f);


    using namespace Rudy;




    Rudy::Log::Init();
    RD_CORE_WARN("test:Initialized Log!");


    //========================================
    //=== initialize renderer
    //auto window = Window::Create();
    //Input::SetWindowContext(window->GetNativeWindow());


    auto main_camera = Camera::Create(MAIN_CAMERA_POS); 

    Renderer::Init(SCR_WIDTH, SCR_HEIGHT);
    Renderer::SetMainCamera(main_camera);

    auto renderAPI =  Renderer::GetAPI();



    //=================================================================================================
    //=== initialize the resources

  
    auto showNormalShader = Shader::Create("normal Shader", 
        "Shaders/Shaders/default_VS.glsl", "Shaders/Shaders/show_normal_FS.glsl",
        "Shaders/Shaders/show_normal_GS.glsl");
    Material::SetMaterialProperties(showNormalShader);



    //======gbuffer pass;

    auto gBufferPassShader = Shader::Create("gBuffer Shader",
        "Shaders/Deferred/GBuffer_VS.glsl", 
         "Shaders/Deferred/GBuffer_FS.glsl");
    Material::SetMaterialProperties(gBufferPassShader);


    //Framebuffer;
    auto GBufferFBO = FrameBuffer::Create(
        SCR_WIDTH, SCR_WIDTH, FrameBufferType::GBuffer, 8);

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



    GBufferFBO->Bind();
    GBufferFBO->SetColorTexture(gPosition, 0);
    GBufferFBO->SetColorTexture(gAlbedo, 1);
    GBufferFBO->SetColorTexture(gWorldNormal, 2);
    GBufferFBO->SetColorTexture(gWorldTangent, 3);
    GBufferFBO->SetColorTexture(gSpecular, 4);
    GBufferFBO->SetColorTexture(gMetallic, 5);
    GBufferFBO->SetColorTexture(gRoughness, 6);

    GBufferFBO->SetDepthTexture(gScreenDepth);

    GBufferFBO->FinishSetup();
    GBufferFBO->Unbind();




    //=====the scene 

    //Scene
    auto scene = Scene::Create();

    //lighting
    auto sunlight = DirectionalLight::Create();
    //sunlight->direction = glm::vec3(0.5f, -0.5f, -1.0f);
    sunlight->direction = glm::vec3(-1.0f, -1.0f, -1.0f); 
 

    //info for shadowmap:
    //the orthographic projection matrix for the light source£º
    glm::mat4 lightProjection = glm::ortho(
        -20.0f, 20.0f, -20.0f, 20.0f, -10.0f, 20.0f);  //the near and far plane should be large enough to cover the scene
    //look at minus direction;
    glm::mat4 lightView = glm::lookAt(- sunlight->direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

    auto lightSpaceCamera = Camera::Create();
    lightSpaceCamera->m_ProjectionMatrix = lightProjection;
    lightSpaceCamera->m_ViewMatrix = lightView;


     

    //plane;
    auto plane_Material = Material::Create(gBufferPassShader);

    //auto plane_albedoMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/albedo.png");
    plane_Material->SetVec3("u_Albedo", glm::vec3(1.0f, 0.0f, 0.0f));
    auto plane_normalMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/normal.png");
    auto plane_roughnessMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/roughness.png");
    auto plane_metallicMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/metallic.png");


   // plane_Material->SetTexture(TexType::AlbedoMap, plane_albedoMap);
    plane_Material->SetTexture(TexType::NormalMap, plane_normalMap);
    plane_Material->SetTexture(TexType::RoughnessMap, plane_roughnessMap);
    plane_Material->SetTexture(TexType::MetallicMap, plane_metallicMap);


    auto floor = Plane::Create(30);
    floor->transform->position = glm::vec3(0.0f, -1.0f, 0.0f);
    floor->SetMaterial(plane_Material);
     
  



    auto sphere = Sphere::Create(20);
    sphere->SetMaterial(plane_Material);

    //from light source to the center of the scene; make sure negative;
    sphere->transform->position = - glm::normalize(sunlight->direction) * glm::vec3(3.0f);
    sphere->transform->scale = glm::vec3(0.2f, 0.2f, 0.2f);


    auto sphere2 = Sphere::Create(20);
    sphere2->SetMaterial(plane_Material); 
    sphere2->transform->position = glm::vec3(2.0f, 0.0f, 0.0f); 





    //==========shadow map pass
    auto shadowMapShader = Shader::Create("shadow map Shader", "Shaders/Shaders/DepthMap_VS.glsl", "Shaders/Shaders/DepthMap_FS.glsl");
    Material::SetMaterialProperties(shadowMapShader);

    auto shadowMapMaterial = Material::Create(shadowMapShader);

    auto shadowMapSkinnedShader = Shader::Create("shadow map Shader", "Shaders/Shaders/DepthMapSkinned_VS.glsl", "Shaders/Shaders/DepthMap_FS.glsl");
    Material::SetMaterialProperties(shadowMapSkinnedShader); 
     auto shadowMapSkinnedMaterial = Material::Create(shadowMapSkinnedShader);


    auto shadowMapFBO = FrameBuffer::Create(
        SHADOW_WIDTH, SHADOW_HEIGHT, FrameBufferType::DepthTexture);

    auto shadowMap = Texture2D::CreateEmpty(
        TextureSpec{ SHADOW_WIDTH, SHADOW_HEIGHT, TextureInternalFormat::DEPTH_COMPONENT24,
                             false, WrapMode::ClampToBorder, FilterMode::Nearest });

    shadowMapFBO->Bind();
    shadowMapFBO->SetDepthTexture(shadowMap);
    shadowMapFBO->FinishSetup();
    shadowMapFBO->Unbind();

   
    //===========new: animated model

     //auto test_model = Model::LoadModel("D:/CG_resources/backpack/backpack.obj");
    auto gBufferPassSkinnedShader = Shader::Create("gBuffer Shader", "Shaders/Deferred/GBufferSkinned_VS.glsl",
        "Shaders/Deferred/GBuffer_FS.glsl");
    Material::SetMaterialProperties(gBufferPassSkinnedShader); 


    Texture2D::SetFlipYOnLoad(true); //eg: for .png;

    Model::s_scaleFactor =  0.01f;
    //auto test_model = Model::LoadModel("D:/CG_resources/byMihoyo/player/player.dae");
    //auto test_model = Model::LoadModel("D:/CG_resources/dae/vampire/dancing_vampire.dae");
    auto test_model = Model::LoadModel("D:/CG_resources/blender_export/player_static.dae");


     

    for (auto meshObj : test_model->meshObjects)
    {
        auto _material = meshObj->GetRendererComponent()->GetMaterial();  
       // _material->SetVec3("u_testColor", glm::vec3(1.0));
        meshObj->transform->rotation =  glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
     
     

    //=================================================================================================
    //======lighting pass

    //auto lightingPassShader = Shader::Create("blinnPhong Shader", "Shaders/Shaders/BlinnPhong_VS.glsl", "Shaders/Shaders/BlinnPhong_FS.glsl");
    //auto lightingPassShader = Shader::Create("NPR Shader", "Shaders/Deferred/Genshin_VS.glsl", "Shaders/Deferred/Genshin_FS.glsl");
   
   // auto lightingPassShader = Shader::Create("NPR Shader", "Shaders/Deferred/PBR_VS.glsl", "Shaders/Deferred/PBR_FS.glsl");
    auto lightingPassShader = Shader::Create("NPR Shader", 
         "Shaders/Shaders/Genshin_VS.glsl", 
         "Shaders/Shaders/Genshin_FS.glsl");
        //"Shaders/Deferred/Lambertian_FS.glsl");
    Material::SetMaterialProperties(lightingPassShader);


    //=== FBO
    auto lightingPassFBO = FrameBuffer::Create(
        SCR_WIDTH, SCR_WIDTH, FrameBufferType::Regular);

    auto lightingPassScreenTexture = Texture2D::CreateEmpty(
        TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F });

    auto viewNormalTexture = Texture2D::CreateEmpty(
		TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGB32F });

    auto depthTexture = Texture2D::CreateEmpty(
		TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::DEPTH_COMPONENT24,
							 false, WrapMode::ClampToBorder, FilterMode::Nearest });

    lightingPassFBO->Bind();
    lightingPassFBO->SetColorTexture(lightingPassScreenTexture, 0);
    //lightingPassFBO->SetColorTexture(viewNormalTexture, 1);
    //lightingPassFBO->SetDepthTexture(depthTexture);
    lightingPassFBO->FinishSetup();
    lightingPassFBO->Unbind();


    //=== material
    Texture2D::SetFlipYOnLoad(true); //eg: for .png;
    auto faceSDF = Texture2D::LoadFile("D:/CG_resources/faceSDF/Avatar_Girl_Tex_FaceLightmap.png");
    auto toonTexture = Texture2D::LoadFile("D:/CG_resources/byMihoyo/player/toon.png");



   

    for (auto meshObj : test_model->meshObjects)
    {
        auto _material = meshObj->GetRendererComponent()->GetMaterial();
        _material->SetTexture(TexType::FaceSDFTexture, faceSDF);
        _material->SetTexture(TexType::ToonTexture, toonTexture); 
        _material->SetBool("u_face", false);
        meshObj->transform->rotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
   
    }



   

    auto lightingPassMaterial = Material::Create(lightingPassShader);

     //lightingPassMaterial->SetTexture(TexType::gPosition, gPosition);
     //lightingPassMaterial->SetTexture(TexType::gAlbedo, gAlbedo);
     //lightingPassMaterial->SetTexture(TexType::gWorldNormal, gWorldNormal);
     //lightingPassMaterial->SetTexture(TexType::gWorldTangent, gWorldTangent);
     //lightingPassMaterial->SetTexture(TexType::gSpecular, gSpecular);
     //lightingPassMaterial->SetTexture(TexType::gMetallic, gMetallic);
     //lightingPassMaterial->SetTexture(TexType::gRoughness, gRoughness);
     //lightingPassMaterial->SetTexture(TexType::gScreenDepth, gScreenDepth);
     
     //lightingPassMaterial->SetTexture(TexType::diffuseEnvMap, diffuseEnvMap);
     //lightingPassMaterial->SetTexture(TexType::specularEnvMap, specularEnvMap);
     //lightingPassMaterial->SetTexture(TexType::brdfLUTMap, brdfLUTMap);
     
     //lightingPassMaterial->SetTexture(TexType::DepthMap, shadowMap);


      

    //--render quad;
    auto lightingPassQuad = ScreenQuad::Create();
    lightingPassQuad->SetMaterial(lightingPassMaterial);
    //

 

 


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

        //a simple orbit sunlight
        //sunlight->direction = glm::normalize(glm::vec3(glm::cos(timer), -1.0f, glm::sin(timer)));
        //sphere->transform->position = - glm::normalize(sunlight->direction) * glm::vec3(3.0f);


        //====== buffer pass: render the scene to the gbuffer 
        if(false)
        {
            GBufferFBO->Bind();

            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content  
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glEnable(GL_DEPTH_TEST);


            //get scene camera info 
            //scene->Render(main_camera);
           // floor->SetShader(gBufferPassShader); 
           // floor->Draw(main_camera); 

            sphere->SetShader(gBufferPassShader);
            sphere->Draw(main_camera);

            sphere2->SetShader(gBufferPassShader);
            sphere2->Draw(main_camera); 

            test_model->SetShader(gBufferPassSkinnedShader); 
            test_model->Draw(main_camera);


            GBufferFBO->Unbind();
        }


        //====== shadowMap pass: render the scene to the shadowMap;
        {
            shadowMapFBO->Bind();

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content 

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

            glEnable(GL_DEPTH_TEST);

            //render the scene to the shadowMap;
            //set the shader as shadowMapShader; 
            floor->SetShader(shadowMapShader);
            floor->Draw(lightSpaceCamera); 


            test_model->SetShader(shadowMapSkinnedShader);
            test_model->Draw(lightSpaceCamera);  //todo:  skinned mesh need special treatment for shadowmap too;

            shadowMapFBO->Unbind();

            glDisable(GL_DEPTH_TEST);

        }


        //======= lighting pass: render the quad;
        {

            lightingPassFBO->Bind();


            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            // glDepthMask(GL_FALSE);
             glEnable(GL_DEPTH_TEST);


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
            //lightingPassShader->SetBool("u_EnableSkyBox", enableSkyBox);

            //lightingPassShader->SetFloat("u_face", false); 
            lightingPassShader->SetVec3("u_testColor", glm::vec3(1.0f));
             

            //lightingPassQuad->Draw(nullptr);
            sphere->SetShader(lightingPassShader);
            sphere->Draw(main_camera);

            sphere2->SetShader(lightingPassShader);
            sphere2->Draw(main_camera);
             
            test_model->SetShader(lightingPassShader);

            auto faceMaterial = test_model->meshObjects[1]->GetRendererComponent()->GetMaterial();
            faceMaterial->SetBool("u_face", true);
            faceMaterial->SetVec3("u_faceForward", glm::vec3(0.0f, 0.0f, 1.0f));
             faceMaterial->SetVec3("u_faceRight", glm::vec3(-1.0f, 0.0f, 0.0f));
            test_model->Draw(main_camera);
  
            lightingPassFBO->Unbind();

            glDisable(GL_DEPTH_TEST);
        }


        //=======postprocessing





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
        // //screenQuadShader->SetBool("u_IsGrayScale", true);
        //  glBindTextureUnit(0, ssrScreenTexture->GetID());  //replace the texture2D here;
        //  screenQuad.Draw(nullptr);

          //screenQuadShader->SetBool("u_IsGrayScale", true);
          //glBindTextureUnit(0, shadowMap->GetID());  //replace the texture 2D here; 
        glBindTextureUnit(0, lightingPassScreenTexture->GetID());  //replace the texture 2D here; 
        screenQuad->Draw(nullptr);

        //screenQuadShader->SetBool("u_IsGrayScale", true);
        //glBindTextureUnit(0, ssrScreenTexture->GetID());  //replace the texture2D here;
         
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        //=======skybox overlay; on final default framebuffer; 
        //compare the depth with gbuffer;  make sure enable the depth test; 
 


        {  
           //glEnable(GL_DEPTH_TEST);
           //glBindFramebuffer(GL_READ_FRAMEBUFFER, GBufferFBO->GetFrameBufferID());
           //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
           //glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
           //
           //test_model->SetShader(showNormalShader);
           //test_model->Draw(main_camera);  
           //
           //sphere ->SetShader(showNormalShader);
           //sphere->Draw(main_camera);
           //
           //
           //sphere2->SetShader(showNormalShader);
           //sphere2->Draw(main_camera);

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

        Renderer::WindowOnUpdate();
        /* Swap front and back buffers */
       // glfwSwapBuffers(window); 
        /* Poll for and process events */
        //glfwPollEvents();



    }


    //====shutdown

    glfwTerminate(); 

}


