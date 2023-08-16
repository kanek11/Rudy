
#include <glad/glad.h>

#include "Ether.h"

#include <iostream>


#include "Hazel/Core/Window.h"

#include "Sphere.hpp"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/GraphicsContext.h"

using namespace Hazel;


int main() {

    Hazel::Log::Init();

    HZ_CORE_WARN("test:Initialized Log!"); 

    
    auto window  = Window::Create();   

    Renderer::Init();
    auto renderAPI = Renderer::s_RendererAPI; 


    Sphere sphere;
    auto sphereMesh = Mesh::Create(sphere.Vertices, sphere.Indices);
   
    sphereMesh->SetupMesh(); 
    sphere.VertexArray = sphereMesh->GetVertexAarry();

     



    //======the loop 
    /* Loop until the user closes the window */
 
    bool isRunning = true;

    while (isRunning) 
    {
        /* Render here */
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
        //glClear(GL_COLOR_BUFFER_BIT);
        renderAPI->SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        renderAPI->Clear();


        sphere.DrawSphere( );


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


