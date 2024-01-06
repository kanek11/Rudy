//ver 2023.12.30 
#include "RudyPCH.h" 
#include <Rudy.h>

#include "Rudy/Particles/ParticleSystem.h"



using namespace Rudy; 
using namespace glm;

bool  visualize_buffer = true; 


//2560:1440 = 16:9
const uint32_t SCR_WIDTH = 2560;
const uint32_t SCR_HEIGHT = 1440;
const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
const uint32_t SHADOW_WIDTH = 2560, SHADOW_HEIGHT = 2560;


const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 2.0f, 5.0f);

 
 


int main() {

    Rudy::Log::Init();
    RD_CORE_WARN("test:Initialized Log!");


    //========================================
    //=== initialize renderer
    //auto window = Window::Create();
    //Input::SetWindowContext(window->GetNativeWindow());


    auto m_camera = Camera::Create(MAIN_CAMERA_POS);

    Renderer::Init(SCR_WIDTH, SCR_HEIGHT);
    Renderer::SetMainCamera(m_camera);

    auto renderAPI = Renderer::s_RendererAPI;



    //=================================================================================================

    //the texture
    auto radial_sprite = Texture2D::LoadFile( "D:/CG_resources/radial_gradient.png")  ;
     

    Emitter* test_particle = new Emitter(); 

    auto m_particle_render_shader = Shader::Create("particle render mesh",
        "Shaders/Particles/particle_render_sprite_VS.glsl",
        "Shaders/Particles/particle_render_sprite_FS.glsl",
        "Shaders/Particles/particle_render_sprite_GS.glsl");

    m_particle_render_shader->Bind();
    m_particle_render_shader->SetInt("u_sprite", 0);
    glBindTextureUnit (0, radial_sprite->GetTextureID());
    
    m_particle_render_shader->SetVec4("u_color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); //red
     
    m_particle_render_shader->SetFloat("u_sprite_size", 0.6f);


    auto particle_material = Material::Create(m_particle_render_shader);
    test_particle->SetMaterial(particle_material);




    auto cube = Cube::Create();

    auto defaultShader = Shader::Create("default Shader", 
        "Shaders/Forward/default_VS.glsl", "Shaders/Forward/default_FS.glsl");
    auto defaultMaterial = Material::Create(defaultShader);
    cube->SetMaterial(defaultMaterial);


    //try draw indirect
    auto drawIndirectBuffer = StorageBuffer::Create();

    struct DrawIndirectCommand {
        uint32_t  count;
        uint32_t  instanceCount = 1;
        uint32_t  first = 0;
        uint32_t  baseInstance = 0;
    };

    auto cmd = new DrawIndirectCommand();
    cmd->count = 36;

    drawIndirectBuffer->SetData(cmd, sizeof(DrawIndirectCommand), BufferUsage::DYNAMIC_COPY);
     
    test_particle->SetMesh(cube->mesh);
    //test_particle->SetMaterial(defaultMaterial);
     
  

    auto lineShader = Shader::Create("line Shader", "Shaders/Forward/Vertex_Color_VS.glsl",
                 "Shaders/Forward/Vertex_Color_FS.glsl");
    Navigation nav = Navigation();
    nav.material = Material::Create(lineShader);
    //
    //
    //
    // auto screenQuadShader = Shader::Create("screen quad shader", "Resources/Shaders/ScreenQuad_VS.glsl", "Resources/Shaders/ScreenQuad_FS.glsl");
    // auto screenQuadMaterial = Material::Create(screenQuadShader);
    //
    // Quad screenQuad = Quad();
    // screenQuad.SetMaterial(screenQuadMaterial);


     //======the loop 
     /* Loop until the user closes the window */


    float lastFrameTime = 0.0f;
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

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // make sure clear the framebuffer's content  
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        //turn on blending
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        test_particle->m_deltaTime = deltaTime;
        test_particle->Update();
        test_particle->Draw(m_camera);
glDisable(GL_BLEND);

      
     


     
       
     
        //=======overlay: world grid; 
        // grid.Draw();  
        nav.Draw();

        m_camera->OnUpdate(deltaTime);
        Renderer::WindowOnUpdate();
        // glfwSwapBuffers(window);  
         //glfwPollEvents();



    }


    //====shutdown 
    Renderer::Shutdown();
    //glfwTerminate();
    return 0;


}


