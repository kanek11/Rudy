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



const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;



using namespace Hazel;


int main() {

    Hazel::Log::Init();

    HZ_CORE_WARN("test:Initialized Log!");


    //auto window = Window::Create();
    //Input::SetWindowContext(window->GetNativeWindow());


    Ref<Camera> camera = CreateRef<Camera>();

    Renderer::Init();
    Renderer::SetMainCamera(camera); 

    auto renderAPI = Renderer::s_RendererAPI;




    //========light sources;

    struct PointLight
    {
    public:
		glm::vec3 Position = glm::vec3(0.0f, 10.0f, 10.0f);
        glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float Intensity = 10;   //scale of the light 
	};


    PointLight pointLight1;

    //compute the light space matrix ;  large fov, aspect ratio same as normalmap;
    float light_FOV = 45.0f, light_AspectRatio = 1.0f, light_Near = 0.1f, light_Far = 100.0f;

    glm::mat4 LightView = glm::lookAt(pointLight1.Position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
    glm::mat4 LightProjection = glm::perspective(light_FOV, light_AspectRatio, light_Near, light_Far);

    glm::mat4 LightViewMatrix = LightProjection * LightView;

      
    auto basicShader = Shader::Create("basic shader", "Resources/Shaders/Basic_VS.glsl", "Resources/Shaders/Basic_FS.glsl");
    
    
    auto blinnPhongShader = Shader::Create("blinn phong", "Resources/Shaders/BlinnPhong_VS.glsl", "Resources/Shaders/BlinnPhong_FS.glsl");
    Material::CreateMaterialType(blinnPhongShader);

    auto depthMapShader = Shader::Create("depth map", "Resources/Shaders/DepthMap_VS.glsl", "Resources/Shaders/DepthMap_FS.glsl");
    Material::CreateMaterialType(depthMapShader);  
    auto depthMapMaterial = Material::Create(depthMapShader);
    
    depthMapShader->Bind();
    depthMapShader->SetMat4("u_Model", glm::mat4(1.0));
    depthMapShader->SetMat4("u_LightViewMat", LightViewMatrix);

 
    
    auto depthQuadShader = Shader::Create("depth quad", "Resources/Shaders/DepthQuad_VS.glsl", "Resources/Shaders/DepthQuad_FS.glsl");
    depthQuadShader->Bind();
    depthQuadShader->SetInt("u_DepthMap", 0);
    depthQuadShader->SetFloat("u_Near_plane", light_Near);
    depthQuadShader->SetFloat("u_Far_plane", light_Far);


    auto skyboxShader = Shader::Create("skybox", "Resources/Shaders/Skybox_VS.glsl", "Resources/Shaders/Skybox_FS.glsl");
    Material::CreateMaterialType(skyboxShader);
    auto skyboxMaterial = Material::Create(skyboxShader); 

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

    skyboxMaterial->SetTexture(TextureType::Skybox, TextureCube::CreateFromImages(paths));

     

    auto test_model = Hierarchy::CreateFromFile( "D:/CG_resources/backpack/backpack.obj"); 
    for (auto meshObj :test_model->m_MeshObjects)
    { 
        meshObj->m_Material->SetShader(blinnPhongShader); 
    }
     

    Plane plane1(10);
    plane1.Transform.Position = glm::vec3(0.0f, -1.5f, 0.0f); 
    auto plane_Material = Material::Create(blinnPhongShader); 
    plane1.Material = plane_Material;

    auto AlbedoMap =   Texture2D::Create("D:/CG_resources/Floor/Albedo.jpg"); 
    //auto AlbedoMap = Texture2D::Create("D:/CG_resources/backpack/diffuse.jpg");
    auto NormalMap =   Texture2D::Create("D:/CG_resources/Floor/Normal.png");
    //auto SpecularMap = Texture2D::Create("D:/CG_resources/Floor/Specular.jpg");
    //auto SpecularMap = Texture2D::Create("D:/CG_resources/backpack/specular.jpg");
    auto RoughnessMap = Texture2D::Create("D:/CG_resources/Floor/Roughness.jpg"); 

    plane_Material->SetTexture(TextureType::AlbedoMap, AlbedoMap); 
    //plane_Material->SetTexture(TextureType::SpecularMap, SpecularMap);
    plane_Material->SetTexture(TextureType::NormalMap, NormalMap);
    plane_Material->SetTexture(TextureType::RoughnessMap, RoughnessMap);

     


    //auto testTexture = Texture2D::Create("C:/Users/10465/Desktop/tsuki4.jpg");



    WorldGrid grid = WorldGrid(20);
    grid.Material = Material::Create(basicShader);

    Quad depthQuad = Quad();
    depthQuad.Shader = depthQuadShader;

    Cube skybox = Cube(); 
    skybox.Material = skyboxMaterial;
   


    auto depthMapFBO = FrameBuffer::CreateWithSize(1024, 1024, FrameBufferType::DepthMap); 

    //======the loop 
    /* Loop until the user closes the window */

    float angle = 0.0f;

    bool isRunning = true;
    float lastFrameTime = 0.0f;


    HZ_CORE_WARN("App: Entering the loop");
    while (! Renderer::ShouldClose()  )
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
          

        //=========first light pass: render the scene to the depth map

  
      
         // enable depth testing ( disabled for rendering screen-space quad)
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        depthMapFBO->Bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content 
        
        
        ////render scene
        for (auto meshObj :  test_model->m_MeshObjects )
            renderAPI->DrawElements(meshObj->m_Mesh, depthMapMaterial, test_model->Transform);
        

        plane1.Material = depthMapMaterial;
        plane1.Draw();  
        
        depthMapShader->Unbind();
        depthMapFBO->Unbind();  
         
        
        //=====second camera pass: render the scene as normal using the generated depth map
        //get the depth map texture from the framebuffer 
        auto depthMapID = depthMapFBO->GetTextureBufferID(0); 
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        
        
        
         
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content 
        
        
        //the scene
        blinnPhongShader->Bind();
        blinnPhongShader->SetMat4("u_LightViewMat", LightViewMatrix);
        blinnPhongShader->SetInt("u_DepthMap", 4);
        glBindTextureUnit(4, depthMapID);
        

        for (auto meshObj : test_model->m_MeshObjects)
            renderAPI->DrawElements(meshObj->m_Mesh, meshObj->m_Material, test_model->Transform);
       
        plane1.Material = plane_Material;
        plane1.Draw();  
        
        //grid.Draw();
        
        blinnPhongShader->Unbind();


        //===skybox

        skybox.DrawSkybox(); 

       
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


