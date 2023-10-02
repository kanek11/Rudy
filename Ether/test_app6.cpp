#include "EtherPCH.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "Hazel/Core/Window.h"

#include "Sphere.hpp"
#include "Plane.hpp"
#include "Quad.hpp"
#include "Cube.hpp"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/FrameBuffer.h"

#include "Hazel/Renderer/Camera.h"

#include "Hazel/Renderer/Object.h"

#include "Hazel/Events/input.h"

#include <stb_image.h>




uint32_t SCR_WIDTH = 2560;
uint32_t SCR_HEIGHT = 1440;

uint32_t BUFFER_WIDTH = SCR_WIDTH/3;
uint32_t BUFFER_HEIGHT = SCR_HEIGHT/3;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;


//g_buffer spec:
// 
//the (int) corrsepond to GL_COLOR_ATTACHMENTi  ;
//working with GPU,  we effectively needs the texture ID ;
 


using namespace Hazel;


int main() {

    Hazel::Log::Init();

    HZ_CORE_WARN("test:Initialized Log!");


    //auto window = Window::Create();
    //Input::SetWindowContext(window->GetNativeWindow());


    Ref<Camera> camera = CreateRef<Camera>();

    Renderer::Init(SCR_WIDTH,SCR_HEIGHT);
    Renderer::SetMainCamera(camera);

    auto renderAPI = Renderer::s_RendererAPI;


    GLint maxDrawBuffers;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
    std::cout << "Max supported draw buffers: " << maxDrawBuffers << std::endl;

    if (maxDrawBuffers >= 2) {
          HZ_CORE_INFO("MRT is supported");
    }
    else {
        HZ_CORE_INFO("MRT is not supported");
    }



   // //Gbuffer setup
   // enum class GBufferTextureType
   // { 
   //     gPosition ,  //0
   //     gAlbedo,    //1
   //     gWorldNormal,  //2
   //     gSpecular,  //3
   //     gMetallic, //4
   //     gRoughness, //5
   //    // gAO,
   //    // g_ShadowMapDepth,
   // };
   // // organize as a dictionary ,  key is the enum,  value is the texture object 


   auto gBufferPassShader = Shader::Create("GBufferShader", "Resources/Shaders/GBuffer_VS.glsl", "Resources/Shaders/GBuffer_FS.glsl");
   Material::CreateMaterialType(gBufferPassShader);  
     


   auto blinnPhongShader = Shader::Create("blinn phong shader", "Resources/Shaders/BlinnPhong_VS.glsl", "Resources/Shaders/BlinnPhong_FS.glsl");
   Material::CreateMaterialType(blinnPhongShader);  


   auto lightingPassMaterial = Material::Create(blinnPhongShader);
   lightingPassMaterial->SetTexture(TextureType::gPosition,    Texture2D::Create(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::RGB32F}));
   lightingPassMaterial->SetTexture(TextureType::gAlbedo,      Texture2D::Create(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::RGB32F})); 
   lightingPassMaterial->SetTexture(TextureType::gWorldNormal, Texture2D::Create(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::RGB32F}));
   lightingPassMaterial->SetTexture(TextureType::gSpecular,    Texture2D::Create(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::R8}));
   lightingPassMaterial->SetTexture(TextureType::gMetallic,    Texture2D::Create(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::R8}));
   lightingPassMaterial->SetTexture(TextureType::gRoughness,   Texture2D::Create(TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureFormat::R8}));
    

   ////HZ_CORE_ERROR("size of lightingpass material textures: {0}", lightingPassMaterial->GetTextures().size());

   auto GBufferFBO = FrameBuffer::Create(SCR_WIDTH, SCR_WIDTH, FrameBufferType::GBuffer, lightingPassMaterial->GetTextures());
 


   ////auto test_model = Hierarchy::CreateFromFile("D:/CG_resources/backpack/backpack.obj");
   ////for (auto meshObj : test_model->m_MeshObjects)
   ////{
   ////    meshObj->m_Material->SetShader(gBufferPassShader);
   ////}


  
    auto plane_Material = Material::Create(gBufferPassShader); 
   
    auto AlbedoMap = Texture2D::Create("D:/CG_resources/Floor/Albedo.jpg"); 
    auto NormalMap = Texture2D::Create("D:/CG_resources/Floor/Normal.png");
    //auto SpecularMap = Texture2D::Create("D:/CG_resources/Floor/Specular.jpg");
    //auto SpecularMap = Texture2D::Create("D:/CG_resources/backpack/roughness.jpg");
     auto RoughnessMap = Texture2D::Create("D:/CG_resources/Floor/Roughness.jpg");
    //auto RoughnessMap = Texture2D::Create("D:/CG_resources/backpack/roughness.jpg");


    plane_Material->SetTexture(TextureType::AlbedoMap, AlbedoMap);
    //plane_Material->SetTexture(TextureType::SpecularMap, SpecularMap);
    plane_Material->SetTexture(TextureType::NormalMap, NormalMap);
    plane_Material->SetTexture(TextureType::RoughnessMap, RoughnessMap);


    Plane floor(10);
    floor.Transform.Position = glm::vec3(0.0f, -1.5f, 0.0f);
    floor.Material = plane_Material;


 



   // //========light sources; 

   // auto basicShader = Shader::Create("basic shader", "Resources/Shaders/Basic_VS.glsl", "Resources/Shaders/Basic_FS.glsl");
     
   // //auto testTexture = Texture2D::Create("C:/Users/10465/Desktop/tsuki4.jpg");
     
   // //WorldGrid grid = WorldGrid(20);
   // //grid.Material = Material::Create(basicShader);
 
 

   // Quad lightingPassQuad = Quad();
   // lightingPassQuad.Material = lightingPassMaterial;

   // HZ_CORE_INFO("lighting pass quad material size: {0}", lightingPassMaterial->GetTextures().size());
   //  
     


    Quad lightingPassQuad = Quad();
    lightingPassQuad.Material = lightingPassMaterial;

    Quad screenQuad = Quad();
    screenQuad.Material = Material::Create(Shader::Create("screen quad shader", "Resources/Shaders/ScreenQuad_VS.glsl", "Resources/Shaders/ScreenQuad_FS.glsl"));
  

    //======the loop 
    /* Loop until the user closes the window */

    float angle = 0.0f;

    bool isRunning = true;
    float lastFrameTime = 0.0f;


    HZ_CORE_WARN("App: Entering the loop");
    while (!Renderer::ShouldClose())
    {
        //get the time of each frame
        float time = (float)glfwGetTime();
        float deltaTime = time - lastFrameTime;
        lastFrameTime = time;

        /* Render here */
        //glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
        //glClear(GL_COLOR_BUFFER_BIT);
        //renderAPI->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //renderAPI->Clear();


        //==========first gbuffer pass: render the scene to the gbuffer

        GBufferFBO->Bind();
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content 
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        //for (auto meshObj : test_model->m_MeshObjects)
        //   renderAPI->DrawElements(meshObj->m_Mesh, meshObj->m_Material, test_model->Transform);
        //HZ_CORE_INFO("GbufferPass");

        floor.Draw();

      
         
        
        GBufferFBO->Unbind();


    
       //========second lighting pass: render the quad using the gbuffer textures
       glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       glDisable(GL_DEPTH_TEST); 
 
       glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
       //HZ_CORE_INFO("screenQuadPass");
       lightingPassQuad.Draw();



        //render the buffers to the screen, for debugging
       //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       glDisable(GL_DEPTH_TEST);

       auto bufferTextures = lightingPassMaterial->GetTextures();
       auto bufferNum = (int)TextureType::gPosition;

       for (int j = 0; j < 3; j++)
           for (int i = 0; i < 3; i++)
           { 
               //if larger than buffer size, then break
               if (i == 1 && j == 1) continue;
               if ( (3 * j + i+1-1 ) > bufferTextures.size()) break;


			   glViewport(i * BUFFER_WIDTH, j * BUFFER_HEIGHT, BUFFER_WIDTH, BUFFER_HEIGHT);  
			   glBindTextureUnit(0, bufferTextures[(TextureType)bufferNum]->GetTextureID());
               bufferNum++;
			   screenQuad.Draw();
		   }

 


         // enable depth testing ( disabled for rendering screen-space quad)
        //glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        //depthMapFBO->Bind();
        //glEnable(GL_DEPTH_TEST);
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content 
        //
        //
        //////render scene
        //for (auto meshObj : test_model->m_MeshObjects)
        //    renderAPI->DrawElements(meshObj->m_Mesh, depthMapMaterial, test_model->Transform);
        //
        //
        //plane1.Material = depthMapMaterial;
        //plane1.Draw();
        //
        //depthMapShader->Unbind();
        //depthMapFBO->Unbind();


        //=====second camera pass: render the scene as normal using the generated depth map
        //get the depth map texture from the framebuffer 
        //auto depthMapID = depthMapFBO->GetTextureBufferID(0);
        //glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); 


     




        //===skybox

        //skybox.DrawSkybox();


        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        //glDisable(GL_DEPTH_TEST);  //just disable the depth test is best practice. should be faster than clearing the depth buffer bit.
        //depthQuadShader->Bind();  
        //glBindTextureUnit(0, depthMapID);   
        ////glBindTextureUnit(0, testTexture->GetTextureID());
        //depthQuad.Draw(); 
        //depthQuadShader->Unbind(); 


        camera->OnUpdate(deltaTime);

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


