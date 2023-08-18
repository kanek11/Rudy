
#include <glad/glad.h>

#include "Ether.h"

#include <iostream>


#include "Hazel/Core/Window.h"

#include "Sphere.hpp"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/Camera.h"

using namespace Hazel;


int main() {

    Hazel::Log::Init();
    HZ_CORE_WARN("test:Initialized Log!"); 

    
    auto window  = Window::Create();   

    Renderer::Init();
    auto renderAPI = Renderer::s_RendererAPI; 


    Sphere sphere;
    auto sphereMesh = Mesh::Create(sphere.Vertices, sphere.Indices);
   
    //sphereMesh->SetupMesh(); 
    sphere.VertexArray = sphereMesh->GetVertexAarry();

     
    auto basicShader = Shader::Create("basic shader", "Resources/Shaders/BasicVS.glsl", "Resources/Shaders/BasicFS.glsl");
    basicShader -> Bind();


    //for now
    Camera camera;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.5f)); 

    std::cout << "View:" << camera.GetViewMatrix();
    std::cout << "Proj:" << camera.GetProjectionMatrix(); 
    
    basicShader->SetMat4("model", model);

    glm ::mat4 view = camera.GetViewMatrix();
    basicShader->SetMat4("view", view );

    glm::mat4 projection = camera.GetProjectionMatrix();
    basicShader->SetMat4("projection", projection);

   
    //======the loop 
    /* Loop until the user closes the window */

    float angle = 0.0f;
 
    bool isRunning = true;

    while (isRunning) 
    {
        /* Render here */
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
        //glClear(GL_COLOR_BUFFER_BIT);
        renderAPI->SetClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        renderAPI->Clear();


        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        sphere.DrawSphere( );

        //turn on wireframe mode
        // 
        ////set it to be twice larger
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::scale(model, glm::vec3(2.0f));
        //angle += 0.5f;
        //model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
        //basicShader->SetMat4("model", model);
        //sphere.DrawSphere();


        window->OnUpdate(); 
        /* Swap front and back buffers */
       //glfwSwapBuffers(window); 
        /* Poll for and process events */
       // glfwPollEvents();

    }


    //====shutdown

    glfwTerminate();
    return 0;


}


