//ver 2023.12.30 
#include "RudyPCH.h" 
#include <Rudy.h>

#include "Particles.h"

#include "Rudy/Particles/ParticleSystem.h" 


using namespace Rudy; 
using namespace glm;


namespace Rudy
{
 
    Particles::Particles() : Application()
    { 
    }

    Ref<Particles> Particles::Create()
    {
        return CreateRef<Particles>();
    }

    void Particles::Init()
    {

    }

 
void Particles::Start() {

    bool  visualize_buffer = true; 

    Rudy::Log::Init();
    RD_CORE_WARN("test:Initialized Log!");


    //========================================
    //=== initialize renderer
    //auto window = Window::Create();
    //Input::SetWindowContext(window->GetNativeWindow());


    auto main_camera = Camera::Create(MAIN_CAMERA_POS);

    RendererApp::Init(SCR_WIDTH, SCR_HEIGHT);
    RendererApp::SetMainCamera(main_camera);

    auto renderAPI = RendererApp::RendererApp::GetAPI();



    //=================================================================================================

    //sprite renderer 
    auto radial_sprite = Texture2D::LoadFile("D:/CG_resources/radial_gradient.png")  ;
     
    auto m_particle_sprite_shader = Shader::Create("particle sprite shader",
        "Shaders/Particles/particle_render_sprite_VS.glsl",
        "Shaders/Particles/particle_render_sprite_FS.glsl",
        "Shaders/Particles/particle_render_sprite_GS.glsl");

    m_particle_sprite_shader->Bind();
    m_particle_sprite_shader->SetInt("u_sprite", 0);
    glBindTextureUnit(0, radial_sprite->GetID());

    m_particle_sprite_shader->SetVec4("u_color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); //red 
    m_particle_sprite_shader->SetFloat("u_sprite_size", 0.6f);
     
    auto particle_sprite_material = Material::Create(m_particle_sprite_shader);
   

    auto particle_sprite_renderer = ParticleSpriteRenderer::Create();
    particle_sprite_renderer->SetMaterial(particle_sprite_material);


    //mesh renderer
    auto m_particle_mesh_shader = Shader::Create("particle mesh shader",
        "Shaders/Particles/particle_render_mesh_VS.glsl",
        "Shaders/Particles/particle_render_mesh_FS.glsl" );

    auto particle_mesh_material = Material::Create(m_particle_mesh_shader);
     

    Emitter* test_particle = new Emitter(); 


    auto particle_mesh_renderer = StaticMeshRenderer::Create(); 
    particle_mesh_renderer->SetMaterial(particle_mesh_material);
 

    auto sphere = Sphere::Create(10); 
    sphere->transform->scale = glm::vec3(0.1f);
    
    auto defaultShader = Shader::Create("default Shader", 
        "Shaders/Shaders/default_VS.glsl", "Shaders/Shaders/default_FS.glsl");
    auto defaultMaterial = Material::Create(defaultShader);
    sphere->GetRenderer()->SetMaterial(defaultMaterial);


    particle_mesh_renderer->SetMesh(sphere->GetRenderer()->GetMesh()); 

     
 
    test_particle->SetRendererComponent(particle_sprite_renderer);
    //test_particle->SetRendererComponent(particle_mesh_renderer);
    test_particle->Spawn(); 




     


    //simple floor
    auto floor = Plane::Create(1);
    floor->transform->scale = glm::vec3(100.0f);
    auto floorShader = Shader::Create("default Shader",
        "Shaders/Shaders/default_VS.glsl", "Shaders/Shaders/default_FS.glsl");

    floorShader->SetBool("u_custom_color", true);
    floorShader->SetVec3("u_color", glm::vec3(0.5f, 0.5f, 0.5f));

    auto floorMaterial = Material::Create(floorShader); 
    floor->GetRenderer()->SetMaterial(floorMaterial);



    //========================================
    auto gridShader = Shader::Create("default Shader", "Shaders/Shaders/Default_VS.glsl", "Shaders/Shaders/Default_Flat_FS.glsl");
    WorldGrid grid = WorldGrid(20);
    grid.material = Material::Create(gridShader);


    auto lineShader = Shader::Create("line Shader",
                 "Shaders/Shaders/Vertex_Color_VS.glsl",
                 "Shaders/Shaders/Vertex_Color_FS.glsl");
    Navigation nav = Navigation();
    nav.material = Material::Create(lineShader);


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

        floor->Draw(main_camera);

        //turn on blending
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        test_particle->m_deltaTime = deltaTime;
        test_particle->Update();
        test_particle->Draw(main_camera);
glDisable(GL_BLEND);

      
      
        //=======overlay: world grid; 
        // grid.Draw();  
        nav.Draw();

        main_camera->OnUpdate(deltaTime);
        RendererApp::WindowOnUpdate();
        // glfwSwapBuffers(window);  
         //glfwPollEvents();



    }


    //====shutdown 
    RendererApp::Shutdown();
    //glfwTerminate(); 


}


}
