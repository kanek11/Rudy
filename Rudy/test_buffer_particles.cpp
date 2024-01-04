//ver 2023.12.30 
#include "RudyPCH.h" 
#include <Rudy.h>



using namespace Rudy; 
bool  visualize_buffer = true;
bool  enableSkyBox = true;
bool  enableSSAO = false;
bool  enableSSR = false;


//2560:1440 = 16:9
const uint32_t SCR_WIDTH = 2560;
const uint32_t SCR_HEIGHT = 1440;
const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
const uint32_t SHADOW_WIDTH = 2560, SHADOW_HEIGHT = 2560;


const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 2.0f, 5.0f);

//const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 0.0f, 3.0f);




const uint32_t ParticleCount = 10;


using namespace glm;

float m_deltaTime = 0.0f;
vec3  m_acceleration      = vec3(0.0, -1.0f, 0.0);



vec3  m_emitter_world_pos = vec3(0.0f);
//mat3  m_emitter_basis =     mat3(1.0f); 

vec2  m_start_position_min_max = vec2(-1.5, 1.5);
vec2  m_start_velocity_min_max = vec2(-1.5, 1.5);

//vec2  m_start_rotational_velocity_min_max = vec2(-15, 15);
//vec3  m_direction_constraints = vec3(1, 1, 1);
//float m_cone_angle = glm::degrees(glm::pi<float>() / 8.0f);
float m_particle_lifetime = 3.0f;
vec3 m_particles_color = vec3(1, 0.474, 0.058);


 



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
   

    auto computeShader = Shader::CreateComputeShader("compute shader", "Resources/ComputeShaders/Particle_CS.glsl");

     

    //Scene
   // auto scene = Scene::Create();
   //
   // //get a mesh model.
     auto model = Cube::Create();
     uint32_t model_VAO_ID = model->GetVertexArray()->GetVertexArrayID(); 

     std::cout << model_VAO_ID << std::endl;

     auto mesh_shader = Shader::Create("mesh shader", "Resources/ComputeShaders/Particle_VS.glsl", "Resources/ComputeShaders/Particle_FS.glsl");
     auto mesh_material = Material::Create(mesh_shader); 
     

    //glVertexArrayVertexBuffer(model_VAO_ID, 0, model->vertexBuffer->GetBufferID(), 0, sizeof(glm::vec2));
    //glEnableVertexArrayAttrib(model_VAO_ID, 0);
    //glVertexArrayAttribFormat(model_VAO_ID, 0, sizeof(glm::vec4), GL_FLOAT, false, 0);
    //glVertexArrayAttribBinding(model_VAO_ID, 0, 0);


     model->mesh->SetupVertices();
     model->vertexArray->Bind();  

     //glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer->GetBufferID());
     //glBufferData(GL_ARRAY_BUFFER, model->mesh->vertices.size() * sizeof(Vertex), model->mesh->vertices.data(), GL_STATIC_DRAW);
     //
     //std::cout << model->vertexBuffer->GetBufferID() << std::endl;
     //std:: cout << model->mesh->vertices.size() << std::endl;
     //
     //
     //glEnableVertexAttribArray(0);
     //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); 
     //
     //glEnableVertexAttribArray(1);
     //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
     //
     //glEnableVertexAttribArray(2);
     //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
     //
     //
     //
     //glBindVertexArray(0);



     //data for particle system; 
     std::vector<glm::vec4> init_pos (ParticleCount, glm::vec4(0.0, 0.0, 0.0, 1.0));
     std::vector<glm::vec4> init_vel(ParticleCount, glm::vec4(0.0f));
     std::vector<float>     init_ages      (ParticleCount);
     std::vector<glm::vec2> init_rot( ParticleCount, glm::vec2(0.0));

     //fill the ages with random values;
     for (int i = 0; i < ParticleCount; i++) {
		 init_ages[i] = (float)rand() / RAND_MAX * m_particle_lifetime;
	 }

     //fill the intial velocity with random values;
     for (int i = 0; i < ParticleCount; i++) {
		 float x = (float)rand() / RAND_MAX;
		 float y = (float)rand() / RAND_MAX;
		 float z = (float)rand() / RAND_MAX;
		 init_vel[i] = glm::vec4(x, y, z, 0.0f) * glm::vec4(2.0)  - glm::vec4(1.0);
	 }
 

     //bufffers;
     auto pos_buffer = StorageBuffer::Create();
     auto vel_buffer = StorageBuffer::Create();
     auto age_buffer = StorageBuffer::Create();
     auto rot_buffer = StorageBuffer::Create();
     

     
     pos_buffer->BindBase(0);
     vel_buffer->BindBase(1);
     age_buffer->BindBase(2);
     rot_buffer->BindBase(3);
     

     
     pos_buffer->SetData(init_pos.data(),  init_pos.size() * sizeof(init_pos[0]),   BufferUsage::DYNAMIC_DRAW);
     vel_buffer->SetData(init_vel.data(),  init_vel.size() * sizeof(init_vel[0]),   BufferUsage::DYNAMIC_DRAW);
     age_buffer->SetData(init_ages.data(), init_ages.size() * sizeof(init_ages[0]), BufferUsage::DYNAMIC_DRAW);
     rot_buffer->SetData(init_rot.data(),  init_rot.size() * sizeof(init_rot[0]),   BufferUsage::DYNAMIC_DRAW);

 

     //positionbuffer
    glVertexArrayVertexBuffer  (model_VAO_ID, 7, pos_buffer->GetBufferID(), 0, sizeof(glm::vec4)); 
    glEnableVertexArrayAttrib  (model_VAO_ID, 7);
    glVertexArrayAttribFormat  (model_VAO_ID, 7, sizeof(glm::vec4), GL_FLOAT, false, 0);
    glVertexArrayAttribBinding (model_VAO_ID, 7, 7);
    glVertexArrayBindingDivisor(model_VAO_ID, 7, 1);
    
    //rotation buffer
    glVertexArrayVertexBuffer  (model_VAO_ID, 8, rot_buffer->GetBufferID(), 0, sizeof(glm::vec2));
    glEnableVertexArrayAttrib  (model_VAO_ID, 8);
    glVertexArrayAttribFormat  (model_VAO_ID, 8, sizeof(glm::vec2), GL_FLOAT, false, 0);
    glVertexArrayAttribBinding (model_VAO_ID, 8, 8);
    glVertexArrayBindingDivisor(model_VAO_ID, 8, 1); 
 

     
    auto lineShader = Shader::Create("line Shader", "Resources/Shaders/Vertex_Color_VS.glsl", "Resources/Shaders/Vertex_Color_FS.glsl");
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


       computeShader->Bind();

       computeShader->SetFloat("u_deltaTime", deltaTime);
       computeShader->SetVec3("u_acceleration", m_acceleration);

       computeShader->SetVec3("u_emitter_world_pos", m_emitter_world_pos); 

       //computeShader->SetMat3("u_emitter_basis", m_emitter_basis);
       computeShader->SetVec2("u_start_position_min_max", m_start_position_min_max);
       computeShader->SetVec2("u_start_velocity_min_max", m_start_velocity_min_max);
        
       computeShader->SetFloat("u_particle_lifetime", m_particle_lifetime);
      // computeShader->SetVec2("u_start_rotational_velocity_min_max", m_start_rotational_velocity_min_max);
      // computeShader->SetVec3("u_direction_constraints", m_direction_constraints);
       //computeShader->SetFloat("u_cone_angle", m_cone_angle); 
 
	   float x = (float)rand() / RAND_MAX;
	   float y = (float)rand() / RAND_MAX;
	   float z = (float)rand() / RAND_MAX;
       computeShader->SetVec3("u_random", vec3(x, y, z));
     
 
       glDispatchCompute(ParticleCount,1, 1);  
       glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        

       computeShader->Unbind();

      // glBindBuffer(GL_SHADER_STORAGE_BUFFER, pos_buffer->GetBufferID());
      // float* data = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
      //
      // std::cout << "position buffer: " << std::endl;
      // int counter = 0;
      // for (int i = 0; i < 8; i++) {
		//   std::cout << data[i] << std::endl; 
	  // }
      // glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

       
      // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

       mesh_shader->Bind();
       mesh_shader->SetVec3("u_diffuse", m_particles_color);
       mesh_shader->SetMat4("u_mvp", m_camera->GetProjectionViewMatrix());
       mesh_shader->SetMat4("u_model_view", m_camera->GetViewMatrix());

       glBindVertexArray(model_VAO_ID);  
        
       glDrawArraysInstanced(GL_TRIANGLES, 0, model->mesh->vertices.size(), ParticleCount);
       
       glBindVertexArray(0);
 	   mesh_shader->Unbind();

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


