#include "EtherPCH.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "Hazel/Core/Window.h"

#include "Sphere.hpp"
#include "Plane.hpp"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/Camera.h"

#include "Hazel/Renderer/Object.h"

#include "Hazel/Events/input.h"

#include <stb_image.h>




using namespace Hazel;


int main() {

    Hazel::Log::Init();

    HZ_CORE_WARN("test:Initialized Log!");


    auto window = Window::Create();
    Input::SetWindowContext(window->GetNativeWindow());


    Ref<Camera> camera = CreateRef<Camera>();

    Renderer::Init();
    Renderer::BeginScene(camera);
    auto renderAPI = Renderer::s_RendererAPI; 


     auto basicShader = Shader::Create("basic shader", "Resources/Shaders/Basic_Vs.glsl", "Resources/Shaders/Basic_Fs.glsl");
   
     

      Object test_model;
      test_model.loadModel("D:/CG_resources/backpack/backpack.obj"); 

     Plane plane1(1);
     plane1.Material = Material::Create(basicShader);

     WorldGrid grid(10);
     grid.Shader = basicShader;

    
     if (plane1.Material !=nullptr )
         HZ_CORE_INFO("material is not empty");

    //======the loop 
    /* Loop until the user closes the window */

    float angle = 0.0f;

    bool isRunning = true;
    float lastFrameTime = 0.0f;

    while (!glfwWindowShouldClose(static_cast<GLFWwindow*>(window->GetNativeWindow())))
    {
        //get the time of each frame
        float time = (float)glfwGetTime();
        float deltaTime = time - lastFrameTime;
        lastFrameTime = time;

        /* Render here */
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
        //glClear(GL_COLOR_BUFFER_BIT);
        renderAPI->SetClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        renderAPI->Clear();

        plane1.Draw();
        grid.Draw();

        //draw the model by loop the meshes and materials of the model
        for (unsigned int i = 0; i < test_model.m_Meshes.size(); i++)  
            renderAPI->DrawElements(test_model.m_Meshes[i], test_model.m_Materials[i], test_model.m_Transform);
        
     

        //renderAPI->DrawElements(sphereMesh, basicMaterial, transform);
        //sphere.DrawSphere();

 

        camera->OnUpdate(deltaTime);

        window->OnUpdate();
        /* Swap front and back buffers */
       // glfwSwapBuffers(window); 
        /* Poll for and process events */
        //glfwPollEvents();

        

    }


    //====shutdown

    glfwTerminate();
    return 0;


}


