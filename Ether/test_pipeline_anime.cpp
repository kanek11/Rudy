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



bool  visualize_buffer = false;
bool  showSkyBox = false;
bool  ssao = false;
bool  ssr = false;


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


    //========================================
    //=== initialize renderer
    //auto window = Window::Create();
    //Input::SetWindowContext(window->GetNativeWindow());


    Ref<Camera> main_camera = CreateRef<Camera>(MAIN_CAMERA_POS);

    Renderer::Init(SCR_WIDTH, SCR_HEIGHT);
    Renderer::SetMainCamera(main_camera);

    auto renderAPI = Renderer::s_RendererAPI;

 

   auto test_model = Model::LoadModel("D:/CG_resources/animation/vampire/dancing_vampire.dae");
   test_model ->meshObjects[0]->transform -> scale = glm::vec3(0.01f, 0.01f, 0.01f);


   auto animation_clip =  test_model->animationClip;

    animation_clip->CalculateKeyframe(0);
    auto transform = animation_clip->GetGlobalTransform("Hips");

 
    /* Loop until the user closes the window */


    float lastFrameTime = 0.0f;
    HZ_CORE_WARN("App: Entering the loop");
    while (!Renderer::ShouldClose())
    {
        //get the time of each frame
        float time = (float)glfwGetTime();
        float deltaTime = time - lastFrameTime;
        lastFrameTime = time;

        /* Render here */
       glClearColor(0.3f, 0.3f, 0.3f, 1.0f); 
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
       glEnable(GL_DEPTH_TEST);


        //====update the animation
        test_model->Draw();
    
  

        //=======skybox overlay; on final default framebuffer; 
        //compare the depth with gbuffer;  make sure enable the depth test;
 
 


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


