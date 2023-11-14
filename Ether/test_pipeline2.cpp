#include "EtherPCH.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <random>


#include "Hazel/Core/Window.h"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/FrameBuffer.h"

#include "Hazel/Renderer/Camera.h"

#include "Hazel/Renderer/Object.h"



#include "Sphere.hpp"
#include "Plane.hpp"
#include "Quad.hpp"
#include "Cube.h"



bool  visualize_buffer = true;
bool  show_skyBox = false;
bool  ssao = false;
bool  ssr = false;
bool env= false;


const uint32_t SCR_WIDTH = 2560;
const uint32_t SCR_HEIGHT = 1440;
const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
const uint32_t SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;


const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 0.0f, 5.0f);

//const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 0.0f, 3.0f);


//g_buffer spec:
// 
//the (int) corrsepond to GL_COLOR_ATTACHMENTi  ;
//working with GPU,  we effectively needs the texture ID ;



using namespace Hazel;


int main() {

    Hazel::Log::Init();
    HZ_CORE_WARN("test:Initialized Log!");


    std::cout<< sizeof(Vertex) << std::endl;

    //========================================
    //=== initialize renderer
    //auto window = Window::Create();
    //Input::SetWindowContext(window->GetNativeWindow());


    Ref<Camera> main_camera = CreateRef<Camera>(MAIN_CAMERA_POS);

    Renderer::Init(SCR_WIDTH, SCR_HEIGHT);
    Renderer::SetMainCamera(main_camera);

    auto renderAPI = Renderer::s_RendererAPI;



    //=================================================================================================
    //=== initialize the resources


    //=======environment map precomputing pass

    if(env)
    { 
    Texture2D::SetFlipYOnLoad(true);
    //auto testHDRI =     Texture2D::LoadFile("D:/CG_resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr"); 
    auto testHDRI = Texture2D::LoadFile("D:/CG_resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr");

    auto envMap = TextureCube::LoadHDRI("D:/CG_resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr");

    // Texture2D::SetFlipYOnLoad(false);
    auto diffuseEnvMap = TextureCube::LoadHDRI("D:/CG_resources/HDRI/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_Env.hdr");

    //Texture2D::SetFlipYOnLoad(true);
    //auto diffuseEnvMap = envMap->CreatePrefilteredEnvMap(envMap,
    //    ConvolutionType::Diffuse); 

    auto specularEnvMap = envMap->CreatePrefilteredEnvMap(envMap,
        ConvolutionType::Specular, 5);


    //the LUT for BRDF
    auto brdfLUTShader = Shader::Create("brdfLUT Shader", "Resources/Shaders/BrdfLUT_VS.glsl", "Resources/Shaders/BrdfLUT_FS.glsl");
    Material::SetMaterialProperties(brdfLUTShader);

    auto brdfQuadMaterial = Material::Create(brdfLUTShader);

    auto brdfLUTMap = Texture2D::CreateEmpty(TextureSpec{ 512, 512, TextureFormat::RGB32F });

    auto brdfLUTFBO = FrameBuffer::Create(
        512, 512, FrameBufferType::Screen);

    brdfLUTFBO->Bind();
    brdfLUTFBO->SetColorAttachmentTexture(brdfLUTMap, 0);

    glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, 512, 512);

    Quad brdfQuad = Quad();
    brdfQuad.SetMaterial(brdfQuadMaterial);
     
    brdfQuad.Draw();

    brdfLUTFBO->Unbind();
     



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

    }




    //======gbuffer pass;

    auto gBufferPassShader = Shader::Create("gBuffer Shader", "Resources/Shaders/GBuffer_VS.glsl", "Resources/Shaders/GBuffer_FS.glsl");
    Material::SetMaterialProperties(gBufferPassShader);


    //========Framebuffer;
    auto GBufferFBO = FrameBuffer::Create(
        SCR_WIDTH, SCR_WIDTH, FrameBufferType::GBuffer, 8);

    //the framebuffer for gbuffer pass: 8;
    auto gPosition = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::RGB32F });
    auto gAlbedo = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::RGB32F });
    auto gWorldNormal = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::RGB32F });
    auto gWorldTangent = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::RGB32F });
    auto gSpecular = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::R32F });
    auto gMetallic = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::R32F });
    auto gRoughness = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::R32F });
    auto gScreenDepth = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::DEPTH_COMPONENT,
                             false, WrapMode::ClampToBorder, FilterMode::Nearest });



    GBufferFBO->Bind();
    GBufferFBO->SetColorAttachmentTexture(gPosition, 0);
    GBufferFBO->SetColorAttachmentTexture(gAlbedo, 1);
    GBufferFBO->SetColorAttachmentTexture(gWorldNormal, 2);
    GBufferFBO->SetColorAttachmentTexture(gWorldTangent, 3);
    GBufferFBO->SetColorAttachmentTexture(gSpecular, 4);
    GBufferFBO->SetColorAttachmentTexture(gMetallic, 5);
    GBufferFBO->SetColorAttachmentTexture(gRoughness, 6);

    GBufferFBO->SetDepthAttachmentTexture(gScreenDepth);

    GBufferFBO->CheckCompleteness();
    GBufferFBO->Unbind();




    //=====the rendering object: scene

    //plane;
    //auto plane_Material = Material::Create(gBufferPassShader);
    //
    //auto plane_albedoMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/albedo.png");
    //auto plane_normalMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/normal.png");
    //auto plane_roughnessMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/roughness.png");
    //auto plane_metallicMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/Floor_brown_ue/metallic.png");
    //
    //
    //plane_Material->SetTexture(TextureType::AlbedoMap, plane_albedoMap);
    //plane_Material->SetTexture(TextureType::NormalMap, plane_normalMap);
    //plane_Material->SetTexture(TextureType::RoughnessMap, plane_roughnessMap);
    //plane_Material->SetTexture(TextureType::MetallicMap, plane_metallicMap);
    //
    //
    //Plane floor(10); 
    //floor.transform->position = glm::vec3(0.0f, -1.0f, 0.0f);
    //floor.SetMaterial(plane_Material);




    //auto sphere_Material = Material::Create(gBufferPassShader);
    //auto sphere_albedoMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/albedo.png");
    //auto sphere_normalMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/normal.png");
    //auto sphere_roughnessMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/roughness.png");
    //auto sphere_metallicMap = Texture2D::LoadFile("D:/CG_resources/PBRTextures/rusted_iron_ue/metallic.png");


    //sphere_Material->SetTexture(TextureType::AlbedoMap, sphere_albedoMap);
    //sphere_Material->SetTexture(TextureType::NormalMap, sphere_normalMap);
    //sphere_Material->SetTexture(TextureType::RoughnessMap, sphere_roughnessMap);
    //sphere_Material->SetTexture(TextureType::MetallicMap, sphere_metallicMap);


    //Sphere sphere = Sphere(20);
    //sphere.SetMaterial(sphere_Material);


     
    //new: animated model

     //auto test_model = Model::LoadModel("D:/CG_resources/backpack/backpack.obj");
    auto gBufferSkinnedPassShader = Shader::Create("gBuffer Shader", "Resources/Shaders/GBufferSkinned_VS.glsl", "Resources/Shaders/GBuffer_FS.glsl");
    Material::SetMaterialProperties(gBufferSkinnedPassShader);



    Texture2D::SetFlipYOnLoad(true); //eg: for .png;

    Model::ScaleFactor = 0.01f;
    //auto test_model = Model::LoadModel("D:/CG_resources/animation/vampire/dancing_vampire.dae");
 
    //Model::ScaleFactor = 0.01f;
   // auto test_model = Model::LoadModel("D:/CG_resources/animation/Catwalk Walk Turn 180 Tight.dae");
    
    //  Model::ScaleFactor = 1.0f;
   auto test_model = Model::LoadModel("C:/Users/10465/Desktop/cat.gltf");
 
    //test: update the global transform of the model; according to the transform of bones;
    //Transform::UpdateWorldTransformRecursive(test_model->rootNode->transform, glm::mat4(1.0f));



    for (auto meshObj : test_model->meshObjects)
    {
        meshObj->material->SetShader(gBufferSkinnedPassShader);
    }
     

    auto transforms = std::vector<glm::mat4>(100, glm::mat4(1.0f));

    //test if the animation works;
    auto test_animation_clip = test_model->animationClip;
    auto animator = Animator::Create();
    if (test_animation_clip) 
    {

        //animation_clip->CalculateKeyframe(0);
//auto transform = animation_clip->GetGlobalTransform("Hips");

//pose it to the first frame; 
        animator->model = test_model;
        animator->animationClip = test_model->animationClip;
          
        transforms = animator->GetBoneTransforms(0.2);

    }


    gBufferSkinnedPassShader->Bind();
    
    //HZ_CORE_INFO("get transforms of size{0}", transforms.size());
    for (int i = 0; i < 100; ++i)
        gBufferSkinnedPassShader->SetMat4("u_BoneTransforms[" + std::to_string(i) + "]", transforms[i]);
    //  gBufferSkinnedPassShader->SetMat4("u_BoneTransforms[" + std::to_string(i) + "]", glm::mat4(1.0f));
    gBufferSkinnedPassShader->SetMat4("u_TestIdentity", glm::mat4(1.0f));


    gBufferSkinnedPassShader->Unbind();







     //=================================================================================================
     //======lighting pass

     //auto lightingPassShader = Shader::Create("blinnPhong Shader", "Resources/Shaders/BlinnPhong_VS.glsl", "Resources/Shaders/BlinnPhong_FS.glsl");
    auto lightingPassShader = Shader::Create("pbr Shader", "Resources/Shaders/PBR_VS.glsl", "Resources/Shaders/PBR_FS.glsl");

    Material::SetMaterialProperties(lightingPassShader);


    //=== FBO
    auto lightingPassFBO = FrameBuffer::Create(
        SCR_WIDTH, SCR_WIDTH, FrameBufferType::Screen);

    auto lightingPassScreenTexture = Texture2D::CreateEmpty(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::RGB32F });

    lightingPassFBO->Bind();
    lightingPassFBO->SetColorAttachmentTexture(lightingPassScreenTexture, 0);
    lightingPassFBO->CheckCompleteness();
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

   // lightingPassMaterial->SetTexture(TextureType::diffuseEnvMap, diffuseEnvMap);
   // lightingPassMaterial->SetTexture(TextureType::specularEnvMap, specularEnvMap);
   // lightingPassMaterial->SetTexture(TextureType::brdfLUT, brdfLUTMap);


    ////HZ_CORE_ERROR("size of lightingpass material textures: {0}", lightingPassMaterial->GetTextures().size());


    //===the rendering objects£º quad;
    Quad lightingPassQuad = Quad();
    lightingPassQuad.SetMaterial(lightingPassMaterial);



    ////auto test_model = Hierarchy::CreateFromFile("D:/CG_resources/backpack/backpack.obj");
    ////for (auto meshObj : test_model->m_MeshObjects)
    ////{
    ////    meshObj->m_Material->SetShader(gBufferPassShader);
    ////}




    //=====skybox pass
    
    if(show_skyBox)
    {
       // auto skyboxShader = Shader::Create("skybox", "Resources/Shaders/Skybox_VS.glsl", "Resources/Shaders/Skybox_FS.glsl");
       // Material::SetMaterialProperties(skyboxShader);
       //
       // //material 
       // auto skyboxMaterial = Material::Create(skyboxShader);
       // skyboxMaterial->SetTexture(TextureType::Skybox, envMap);
       //  
       //  Cube skybox;
       // skybox.SetMaterial(skyboxMaterial);
    }
   




    //========postprocess: SSAO pass;

    if (ssao)
    {

    auto ssaoShader = Shader::Create("ssao Shader", "Resources/Shaders/SSAO_VS.glsl", "Resources/Shaders/SSAO_FS.glsl");
    Material::SetMaterialProperties(ssaoShader);

    //material: ssao takes worldpos, worldnormal, 
    //make sure use a consistent space for the depth;  
    // custom sampling kernel, rotation noise texture



    //refer to https://learnopengl.com/Advanced-Lighting/SSAO

    auto m_Lerp = [](float a, float b, float f) -> float {
        return a + f * (b - a);   };

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (uint32_t i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = m_Lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }


    //used by all fragments;
    ssaoShader->Bind();
    for (uint32_t i = 0; i < 64; ++i)
        ssaoShader->SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);


    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> ssaoNoise;
    for (uint32_t i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    auto ssaoNoiseTexture = Texture2D::CreateUsingData(TextureSpec{
            4, 4, TextureFormat::RGB32F, false, WrapMode::Repeat, FilterMode::Nearest },
            ssaoNoise.data());



    auto ssaoMaterial = Material::Create(ssaoShader);
    ssaoMaterial->SetTexture(TextureType::gPosition, gPosition);
    ssaoMaterial->SetTexture(TextureType::gWorldNormal, gWorldNormal);
    ssaoMaterial->SetTexture(TextureType::gWorldTangent, gWorldTangent);
    ssaoMaterial->SetTexture(TextureType::gScreenDepth, gScreenDepth);
    ssaoMaterial->SetTexture(TextureType::NoiseTexture, ssaoNoiseTexture);


    //-------the quad for ssao pass;
    Quad ssaoQuad = Quad();
    ssaoQuad.SetMaterial(ssaoMaterial);


    //FBO, one-channel;
    auto ssaoFBO = FrameBuffer::Create(
        SCR_WIDTH, SCR_WIDTH, FrameBufferType::Screen);

    auto ssaoScreenTexture = Texture2D::CreateEmpty(
        TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::R32F,
                     false, WrapMode::ClampToBorder, FilterMode::Nearest });

    ssaoFBO->Bind();
    ssaoFBO->SetColorAttachmentTexture(ssaoScreenTexture, 0);
    ssaoFBO->CheckCompleteness();
    ssaoFBO->Unbind();





    }




    //==========SSR; 

    if (ssr)
    {

        auto ssrShader = Shader::Create("ssr Shader", "Resources/Shaders/SSR_VS.glsl", "Resources/Shaders/SSR_FS.glsl");
        Material::SetMaterialProperties(ssrShader);

        ssrShader->Bind();
        ssrShader->SetMat4("u_ProjectionView", main_camera->GetProjectionViewMatrix());
        ssrShader->SetVec3("u_CameraPos", main_camera->GetPosition());

        auto ssrMaterial = Material::Create(ssrShader);
        ssrMaterial->SetTexture(TextureType::gPosition, gPosition);
        ssrMaterial->SetTexture(TextureType::gWorldNormal, gWorldNormal);
        ssrMaterial->SetTexture(TextureType::gScreenDepth, gScreenDepth);
        ssrMaterial->SetTexture(TextureType::lightingPassTexture, lightingPassScreenTexture);


        auto ssrQuad = Quad();
        ssrQuad.SetMaterial(ssrMaterial);


        auto ssrFBO = FrameBuffer::Create(
            SCR_WIDTH, SCR_WIDTH, FrameBufferType::Screen);

        auto ssrScreenTexture = Texture2D::CreateEmpty(
            TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::RGB32F,
                         false, WrapMode::ClampToBorder, FilterMode::Linear });

        ssrFBO->Bind();
        ssrFBO->SetColorAttachmentTexture(ssrScreenTexture, 0);
        ssrFBO->CheckCompleteness();
        ssrFBO->Unbind();


    }

    // //WorldGrid grid = WorldGrid(20);
    // //grid.Material = Material::Create(basicShader);



    // Quad lightingPassQuad = Quad();
    // lightingPassQuad.Material = lightingPassMaterial;

    // HZ_CORE_INFO("lighting pass quad material size: {0}", lightingPassMaterial->GetTextures().size());
    //  


    auto screenQuadShader = Shader::Create("screen quad shader", "Resources/Shaders/ScreenQuad_VS.glsl", "Resources/Shaders/ScreenQuad_FS.glsl");
    auto screenQuadMaterial = Material::Create(screenQuadShader);

    Quad screenQuad = Quad();
    screenQuad.SetMaterial(screenQuadMaterial);





    //======the loop 
    /* Loop until the user closes the window */


    float lastFrameTime = (float)glfwGetTime();
    float timer = 0.0f;
    HZ_CORE_WARN("App: Entering the loop");
    while (!Renderer::ShouldClose())
    {
        //get the time of each frame
        float time = (float)glfwGetTime();
        float deltaTime = time - lastFrameTime;
        lastFrameTime = time;
        timer += deltaTime;

        /* Render here */
        //glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
        //glClear(GL_COLOR_BUFFER_BIT);
        //renderAPI->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //renderAPI->Clear();


        //====== buffer pass: render the scene to the gbuffer 
        {
            GBufferFBO->Bind();

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content 

            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            glEnable(GL_DEPTH_TEST);


            //floor.Draw();
            //sphere.Draw();

            //todo: more efficent transfer of the array;
        

           gBufferSkinnedPassShader->Bind();
           
           //std::cout << timer<< std::endl;
           auto transforms = animator->GetBoneTransforms(timer); 
           for (int i = 0; i < 100; ++i)
               gBufferSkinnedPassShader->SetMat4("u_BoneTransforms[" + std::to_string(i) + "]", transforms[i]);
           
            gBufferSkinnedPassShader->Unbind();


            test_model->Draw(); 


            GBufferFBO->Unbind();
        }




        //======= lighting pass: render the quad;
        {

            lightingPassFBO->Bind();


            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            // glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);


            lightingPassShader->Bind();
            lightingPassShader->SetVec3("u_CameraPos", main_camera->GetPosition());
            // HZ_CORE_INFO("camera pos{0}", main_camera->GetPosition());

            lightingPassQuad.Draw();


            // glDepthMask(GL_TRUE); 
            glEnable(GL_DEPTH_TEST);


            lightingPassFBO->Unbind();


        }

         

        //=======postprocessing
            //=====SSAO
        if (ssao)
        {
            //ssaoFBO->Bind();
      //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
      //glDisable(GL_DEPTH_TEST); 
      //glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
      //
      //ssaoShader->Bind();
      //ssaoShader->SetMat4("u_ProjectionView", main_camera->GetProjectionViewMatrix()); 
      //ssaoShader->SetMat4("u_View", main_camera->GetViewMatrix());
      //ssaoShader->SetMat4("u_Projection", main_camera->GetProjectionMatrix());
      //
      //ssaoQuad.Draw(); 
      //
      //ssaoFBO->Unbind();
        }
    
      



        //=====SSR
        // 
        if (ssr)
        {
            //ssrFBO->Bind();
      //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
      //glDisable(GL_DEPTH_TEST); 
      //glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
      //
      //ssrShader->Bind();
      //ssrShader->SetMat4("u_ProjectionView", main_camera->GetProjectionViewMatrix()); 
      //ssrShader->SetMat4("u_View", main_camera->GetViewMatrix());
      //ssrShader->SetMat4("u_Projection", main_camera->GetProjectionMatrix());
      //ssrShader->SetVec3("u_CameraPos", main_camera->GetPosition()); 
      ////HZ_CORE_INFO("cameraPos: {0}", main_camera->GetPosition());
      //ssrQuad.Draw();  
      //ssrFBO->Unbind();


        }
      


        //==========on default framebuffer;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //===========debug: visualize any texture; 

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        screenQuadShader->Bind();
        //screenQuadShader->SetBool("u_IsGrayScale", true);
        glBindTextureUnit(0, lightingPassScreenTexture->GetTextureID());  //replace the texture2D here;

        screenQuad.Draw();

        glEnable(GL_DEPTH_TEST);


        //=======skybox overlay; on final default framebuffer; 
        //compare the depth with gbuffer;  make sure enable the depth test;

        if (show_skyBox)
        {
           // glEnable(GL_DEPTH_TEST);
           // glBindFramebuffer(GL_READ_FRAMEBUFFER, GBufferFBO->GetFrameBufferID());
           // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
           // glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
           // skybox.DrawSkybox();
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
                screenQuad.Draw();
                index++;

            }

            glDisable(GL_DEPTH_TEST);
        }
         


        main_camera->OnUpdate(deltaTime);

        Renderer::WindowOnUpdate();
        /* Swap front and back buffers */
       // glfwSwapBuffers(window); 
        /* Poll for and process events */
        //glfwPollEvents();



    }


    //====shutdown

    glfwTerminate();
    return 0;


}


