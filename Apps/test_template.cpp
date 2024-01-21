//ver 2023.12.30


#include "RudyPCH.h" 

#include "Rudy.h"
 




//const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 0.0f, 3.0f);


//g_buffer spec:
// 
//the (int) corrsepond to GL_COLOR_ATTACHMENTi  ;
//working with GPU,  we effectively needs the texture ID ;



using namespace Rudy;


void StartTemp() {

    //2560:1440 = 16:9
    const uint32_t SCR_WIDTH = 2560;
    const uint32_t SCR_HEIGHT = 1440;
    const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
    const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
    const uint32_t SHADOW_WIDTH = 2560, SHADOW_HEIGHT = 2560;


    const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 2.0f, 5.0f);



    Rudy::Log::Init();
    RD_CORE_WARN("test:Initialized Log!");


    //========================================
    //=== initialize renderer
    //auto window = Window::Create();
    //Input::SetWindowContext(window->GetNativeWindow());


    auto main_camera = Camera::Create(MAIN_CAMERA_POS);

    Renderer::Init(SCR_WIDTH, SCR_HEIGHT);
    Renderer::SetMainCamera(main_camera);

    auto renderAPI = Renderer::GetAPI();



    //=================================================================================================
  
    //=====the scene 

    //Scene
    auto scene = Scene::Create();

   

    //=================================================================================================
    //======lighting pass 

    std::vector<uint32_t> indices = { 0,1,2 };
    auto id0 = indices[0];          
    auto id1= indices[1];

    std::cout << ( id0-id1)  << std::endl;


    //=================================================================================================

    auto defaultShader = Shader::Create("default Shader", "Shaders/Shaders/Default_VS.glsl", "Shaders/Shaders/Default_Flat_FS.glsl");
    WorldGrid grid = WorldGrid(20);
    grid.material = Material::Create(defaultShader);


    auto lineShader = Shader::Create("vertex color Shader", "Shaders/Shaders/Vertex_Color_VS.glsl", "Shaders/Shaders/Vertex_Color_FS.glsl");
    Navigation nav = Navigation();
    nav.material = Material::Create(lineShader);


    auto screenQuadShader = Shader::Create("screen quad shader", "Shaders/Shaders/ScreenQuad_VS.glsl", "Shaders/Shaders/ScreenQuad_FS.glsl");
    auto screenQuadMaterial = Material::Create(screenQuadShader);

    ScreenQuad screenQuad = ScreenQuad();
    screenQuad.SetMaterial(screenQuadMaterial);


    //======the loop 
    /* Loop until the user closes the window */


    float lastFrameTime = (float)glfwGetTime();
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
 

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);



        //=======skybox overlay; on final default framebuffer; 
        //compare the depth with gbuffer;  make sure enable the depth test; 
 
  
  



        //=======overlay: world grid; 
        // grid.Draw();  
        nav.Draw();


        main_camera->OnUpdate(deltaTime);

        Renderer::WindowOnUpdate();
        /* Swap front and back buffers */
       // glfwSwapBuffers(window); 
        /* Poll for and process events */
        //glfwPollEvents();



    }


    //====shutdown

    glfwTerminate(); 


}


