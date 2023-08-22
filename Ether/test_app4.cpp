#include "EtherPCH.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "Hazel/Core/Window.h"

#include "Sphere.hpp"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/Camera.h"

#include "Hazel/Renderer/Object.h"

#include <stb_image.h>

using namespace Hazel;


int main() {

    Hazel::Log::Init();

    HZ_CORE_WARN("test:Initialized Log!");


    auto window = Window::Create();


    Ref<Camera> camera = CreateRef<Camera>();

    Renderer::Init();
    Renderer::BeginScene(camera);
    auto renderAPI = Renderer::s_RendererAPI;



    //auto basicShader = Shader::Create("basic shader", "Resources/Shaders/BasicVs.glsl", "Resources/Shaders/BasicFs.glsl");
    //basicShader -> Bind(); 

     

     Object test_model;
     test_model.loadModel("D:/CG_resources/backpack/backpack.obj");

 

    {
    HZ_PROFILE_SCOPE("test");

    int sum = 0;
    for (int i = 0; i < 1000; i++)
	{
        sum += i;
	} 
    }


    //======the loop 
    /* Loop until the user closes the window */

    float angle = 0.0f;

    bool isRunning = true;

    while (isRunning)
    {
        /* Render here */
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
        //glClear(GL_COLOR_BUFFER_BIT);
        renderAPI->SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        renderAPI->Clear();


        //draw the model by loop the meshes and materials of the model
         for (unsigned int i = 0; i < test_model.m_Meshes.size(); i++)
         {
             //bind the material of each mesh
             test_model.m_Meshes[i]->Bind();
             test_model.m_Materials[i]->Bind();
             //draw the mesh
             renderAPI->DrawElements(test_model.m_Meshes[i], test_model.m_Materials[i], test_model.m_Transform);
         }

        //renderAPI->DrawElements(sphereMesh, basicMaterial, transform);
        //sphere.DrawSphere();


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //renderAPI->DrawElements(sphereMesh, basicMaterial, transform); 
        //turn on wireframe mode
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        ////set it to be twice larger
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::scale(model, glm::vec3(2.0f));
        //angle += 0.5f;
        //model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
        //basicShader->SetMat4("model", model);
        //sphere.DrawSphere();


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


