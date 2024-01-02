//ver 2023.12.30


#include "RudyPCH.h" 

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <random> 

#include "Rudy/Core/Window.h" 

#include "Rudy/Renderer/Renderer.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Shader.h" 
#include "Rudy/Renderer/FrameBuffer.h" 
#include "Rudy/Renderer/Camera.h" 
#include "Rudy/Renderer/Object.h" 


#include "Rudy/Renderer/Light.h"

#include "Rudy/Renderer/Scene.h"

#include "Sphere.hpp"
#include "Plane.hpp"
#include "Quad.hpp"
#include "Cube.h"

#include "Overlay.hpp"


bool  visualize_buffer = true;
bool  enableSkyBox = true;
bool  enableSSAO = false;
bool  enableSSR = false;


//2560:1440 = 16:9
const uint32_t SCR_WIDTH = 2560;
const uint32_t SCR_HEIGHT = 1440;
const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
const uint32_t SHADOW_WIDTH = 2560, SHADOW_HEIGHT = 2560;


const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 2.0f, 5.0f);

//const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 0.0f, 3.0f);


//g_buffer spec:
// 
//the (int) corrsepond to GL_COLOR_ATTACHMENTi  ;
//working with GPU,  we effectively needs the texture ID ;



using namespace Rudy;


int main() {

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


    //======gbuffer pass;



    //=====the scene 

    //Scene
    auto scene = Scene::Create();



    //=================================================================================================
    //======lighting pass 

    //=== material


    //--render quad;


    //=====skybox pass  

    //=================================================================================================

    
    //test: compute shader;
    auto computeShader = Shader::CreateComputeShader("compute shader", "Resources/ComputeShaders/TestImage_CS.glsl");

    //texture for compute shader result, an image.
    auto computeTexture = Texture2D::CreateEmpty(
    TextureSpec{ SCR_WIDTH, SCR_HEIGHT, TextureInternalFormat::RGBA32F, 
    false, WrapMode::ClampToEdge, FilterMode::Linear }); 
 
    auto ID = computeTexture->GetTextureID();
    glBindImageTexture(0, ID , 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID);



    auto defaultShader = Shader::Create("default Shader", "Resources/Shaders/Default_VS.glsl", "Resources/Shaders/Default_Flat_FS.glsl");
    WorldGrid grid = WorldGrid(20);
    grid.material = Material::Create(defaultShader);


    auto lineShader = Shader::Create("vertex color Shader", "Resources/Shaders/Vertex_Color_VS.glsl", "Resources/Shaders/Vertex_Color_FS.glsl");
    Navigation nav = Navigation();
    nav.material = Material::Create(lineShader);



    auto screenQuadShader = Shader::Create("screen quad shader", "Resources/Shaders/ScreenQuad_VS.glsl", "Resources/Shaders/ScreenQuad_FS.glsl");
    auto screenQuadMaterial = Material::Create(screenQuadShader);

    Quad screenQuad = Quad();
    screenQuad.SetMaterial(screenQuadMaterial);


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

        /* Render here */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content  
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glEnable(GL_DEPTH_TEST);



        //test compute
        computeShader->Bind();
        computeShader->SetFloat("t", time);
        computeShader->SetFloat("width", SCR_WIDTH);
        glDispatchCompute((unsigned int)SCR_WIDTH / 10, (unsigned int)SCR_HEIGHT / 10, 1);

        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     

        //====== buffer pass: render the scene to the gbuffer 
        {
        }


        //======= lighting pass: render the quad;
        {
        }


        //=======postprocessing 


        //==========on default framebuffer;
        //===========debug: visualize any texture; 
        //todo: make a good filter for ss pass;  
        // add a "composer"  or design the blend mode;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST); 
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);



        screenQuadShader->Bind();
        screenQuadShader->SetInt("tex", 0); 
         
        screenQuad.Draw(nullptr);



        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);



        //=======skybox overlay; on final default framebuffer; 
        //compare the depth with gbuffer;  make sure enable the depth test; 






        //=======overlay: world grid; 
        // grid.Draw();  
        nav.Draw();


        main_camera->OnUpdate(deltaTime);


        Renderer::WindowOnUpdate(); 
       // glfwSwapBuffers(window);  
        //glfwPollEvents();



    }


    //====shutdown 
    Renderer::Shutdown();
    //glfwTerminate();
    return 0;


}


