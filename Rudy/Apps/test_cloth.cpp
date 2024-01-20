//ver 2023.12.30


#include "RudyPCH.h" 

#include "Rudy.h"


#include "Rudy/Physics/Cloth.h"



//const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 0.0f, 3.0f);


//g_buffer spec:
// 
//the (int) corrsepond to GL_COLOR_ATTACHMENTi  ;
//working with GPU,  we effectively needs the texture ID ;



using namespace Rudy;


void StartCloth() {


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
    //=== initialize the resources
 


    //=====the scene 

    //Scene
    auto scene = Scene::Create();  

    auto plane = Plane::Create(20); 
    auto plane_mesh = plane->GetRendererComponent()->GetMesh();  

    auto defaultShader = Shader::Create("default Shader", "Shaders/Shaders/default_VS.glsl", 
        "Shaders/Shaders/default_FS.glsl"); 
    auto defaultMaterial = Material::Create(defaultShader);
    plane->SetMaterial(defaultMaterial);


    auto cloth = new Cloth(); 
    cloth ->m_mesh = plane_mesh;

    auto defaultClothShader = Shader::Create("default Shader", 
        "Shaders/Cloth/default_cloth_VS.glsl",
        "Shaders/Cloth/default_cloth_FS.glsl");
    auto defaultClothMaterial = Material::Create(defaultClothShader);
     

    auto showNormalShader = Shader::Create("normal Shader",
        "Shaders/Shaders/default_VS.glsl", "Shaders/Shaders/show_normal_FS.glsl",
        "Shaders/Shaders/show_normal_GS.glsl");
     auto showNormalMaterial = Material::Create(showNormalShader);
 
    cloth ->m_material = defaultClothMaterial;
    cloth->Init();


    //=================================================================================================
    //======lighting pass 

    //=== material


    //--render quad;


    //=====skybox pass  




    //=================================================================================================
 


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

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content  
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glEnable(GL_DEPTH_TEST);


        //====== buffer pass: render the scene to the gbuffer 
        {
        }


        //======= lighting pass: render the quad;
        {
            //plane->Draw(main_camera);

            cloth->m_system_dt = deltaTime;
            cloth->Update();  
            cloth->m_material = defaultClothMaterial;
            cloth->Draw(main_camera);

            //cloth->m_material =  showNormalMaterial;
            //cloth->Draw(main_camera);

        }


        //=======postprocessing 


        //==========on default framebuffer;
        //===========debug: visualize any texture; 
        //todo: make a good filter for ss pass;  
        // add a "composer"  or design the blend mode;
         


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


