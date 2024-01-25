//ver 2023.12.30 
#include "RudyPCH.h"  
#include "Rudy.h"
 
 
#include "Rudy/Physics/Cloth.h"
#include "Apps/ClothApp.h"
 

namespace Rudy
{

    ClothApp::ClothApp() : Application()
    {
    }

    Ref<ClothApp> ClothApp::Create()
    {
        return CreateRef<ClothApp>();
    }

    void ClothApp::Init()
    {

    }



    void ClothApp::Start() {
         
         
        Rudy::Log::Init();
        RD_CORE_WARN("test:Initialized Log!");


        //========================================
        //=== initialize renderer
        //auto window = Window::Create();
        //Input::SetWindowContext(window->GetNativeWindow());


        auto main_camera = Camera::Create(MAIN_CAMERA_POS);

        RendererApp::Init(SCR_WIDTH, SCR_HEIGHT);
        RendererApp::SetMainCamera(main_camera);

        auto renderAPI = RendererApp::GetAPI();



        //=================================================================================================
        //=== initialize the resources



        //=====the scene 

        //Scene
        auto scene = Scene::Create();

        auto plane = Plane::Create(20);
        auto plane_mesh = plane->GetRenderer()->GetMesh();

        auto defaultShader = Shader::Create("default Shader", "Shaders/Shaders/default_VS.glsl",
            "Shaders/Shaders/default_FS.glsl");
        auto defaultMaterial = Material::Create(defaultShader);
        plane->SetMaterial(defaultMaterial);


        auto cloth = new Cloth();
        cloth->m_mesh = plane_mesh;

        auto defaultClothShader = Shader::Create("default Shader",
            "Shaders/Cloth/default_Cloth_VS.glsl",
            "Shaders/Cloth/default_Cloth_FS.glsl");
        auto defaultClothMaterial = Material::Create(defaultClothShader);


        auto showNormalShader = Shader::Create("normal Shader",
            "Shaders/Shaders/default_VS.glsl", "Shaders/Shaders/show_normal_FS.glsl",
            "Shaders/Shaders/show_normal_GS.glsl");
        auto showNormalMaterial = Material::Create(showNormalShader);

        cloth->m_material = defaultClothMaterial;
        cloth->Init();




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
        while (!RendererApp::ShouldClose())
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

                //ClothApp->m_material =  showNormalMaterial;
                //ClothApp->Draw(main_camera);

              
                //reset if push the space key;
                if (Input::IsKeyPressed(Key::Space))
				{
					//cloth->Reset();
				}

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

            RendererApp::WindowOnUpdate();
            /* Swap front and back buffers */
           // glfwSwapBuffers(window); 
            /* Poll for and process events */
            //glfwPollEvents();



        }


        //====shutdown

        glfwTerminate();

    }


}