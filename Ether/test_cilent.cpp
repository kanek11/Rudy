
#include <glad/glad.h>

#include "Ether.h"

#include <iostream>


#include "Hazel/Core/Window.h"
using namespace Hazel; 


int main() {

	Hazel::Log::Init();

	HZ_CORE_WARN("test:Initialized Log!"); 



    // Window::Create();

    //initialize glad.
  


 
     GLFWwindow* window;

     ///* Initialize the library */
     if (!glfwInit())
        return -1;

     //* Create a windowed mode window and its OpenGL context */
     window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
     if (!window)
     {
         glfwTerminate();
         return -1;
     }
     //==Init window

     //====init context

     ///* Make the window's context current */
     glfwMakeContextCurrent(window);

     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
     {
         HZ_CORE_ERROR("Failed to initialize GLAD");
         return -1;
     } 

     //====init context


     //draw a triangle
     float vertices[] = {
         -0.5f, -0.5f, 0.0f,
          0.5f, -0.5f, 0.0f,
          0.0f,  0.5f, 0.0f
     };

     //caution: after initialize glad, otherwise there's potential runtime error.
     unsigned int VBO,VAO;
     glGenBuffers(1, &VBO);
     std::cout << VBO << std::endl;
      
     glGenVertexArrays(1, &VAO);
     glBindVertexArray(VAO);
     std::cout << VAO << std::endl;

     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 

     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

 
 
     HZ_CORE_INFO(glGetString(GL_VENDOR));

     //======the loop 
     /* Loop until the user closes the window */
     while (!glfwWindowShouldClose(window))
     {
         /* Render here */
         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT);


         glBindVertexArray(VAO);
         glDrawArrays(GL_TRIANGLES, 0, 3);



         /* Swap front and back buffers */
         glfwSwapBuffers(window);

         /* Poll for and process events */
         glfwPollEvents();
     }


     //====shutdown

     glfwTerminate();
     return 0;


}


 