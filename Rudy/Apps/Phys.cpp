// update 2024.1.22
#include "RudyPCH.h"
#include "Phys.h"

namespace Rudy
{

Phys::Phys() :
    ViewportLayer()
{
    // this->GetWindow()->SetVSync(false);
}

void Phys::ShutDown()
{
    // all lifetime management is done by the smart pointers;
}

void Phys::OnImGuiRender()
{
    ImGui::Begin("Phys");

    // check enable particles
    ImGui::Checkbox("Enable Particles", &enableParticles);
    ImGui::Checkbox("Enable Cloth", &enableCloth);
    ImGui::Checkbox("Enable Fluid", &enableFluid);

    // switch emisson shape
    ImGui::Text("Emission Shape");
    ImGui::RadioButton("sphere", &particle->m_emission_shape, 0);
    ImGui::RadioButton("cone", &particle->m_emission_shape, 1);

    // fluid solver;
    ImGui::Text("Fluid Solver");
    ImGui::SliderFloat("SOR", &fluid->m_over_relaxation, 1.0f, 2.0f);

    // for unsigned int

    if (ImGui::SliderInt("Iterations", &fluid->iter_temp, 100, 1000))
    {
        fluid->m_num_iter = static_cast<uint32_t>(fluid->iter_temp);
    }

    ImGui::End();
}

void Phys::Init()
{
    ViewportLayer::Init();

    //=====
    this->particle = Emitter::Create();

    particle->Spawn();

    particle->m_emission_shape       = 1; // cone
    particle->m_cone_angle           = 5.0f;
    particle->m_emission_rate        = 200;
    particle->m_particle_minLifetime = 1.5f;
    particle->m_particle_maxLifetime = 4.5f;

    // simple floor
    this->floor                   = Plane::Create(10);
    this->floor->transform->scale = glm::vec3(100.0f);
    auto floorShader              = Shader::Create("default Shader",
                                      "Shaders/Shaders/default_VS.glsl",
                                      "Shaders/Shaders/default_FS.glsl");

    floorShader->Bind();
    floorShader->SetBool("u_use_color", true);
    floorShader->SetVec3("u_color", glm::vec3(0.5f));

    auto floorMaterial = Material::Create(floorShader);
    floor->SetMaterial(floorMaterial);

    // ======fluid
    //================================================================================================
    fluid = new Fluid();
    fluid->Init();

    // ===cloth
    //================================================================================================
    auto plane      = Plane::Create(20);
    auto plane_mesh = plane->GetRenderer()->GetMesh();

    this->cloth   = new Cloth();
    cloth->m_mesh = plane_mesh;

    auto defaultClothShader   = Shader::Create("default Shader",
                                             "Shaders/Cloth/default_Cloth_VS.glsl",
                                             "Shaders/Cloth/default_Cloth_FS.glsl");
    auto defaultClothMaterial = Material::Create(defaultClothShader);

    auto showNormalShader   = Shader::Create("normal Shader",
                                           "Shaders/Shaders/default_VS.glsl",
                                           "Shaders/Shaders/show_normal_FS.glsl",
                                           "Shaders/Shaders/show_normal_GS.glsl");
    auto showNormalMaterial = Material::Create(showNormalShader);

    cloth->m_material = defaultClothMaterial;
    cloth->Init();
}

void Phys::OnUpdate(float deltaTime)
{
    /* Render here */

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // make sure clear the framebuffer's content
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    if (enableParticles)
    {
        floor->Draw(this->GetMainCamera());

        // turn on blending
        particle->m_deltaTime = deltaTime;
        particle->Update();
        particle->Draw(this->GetMainCamera());
    }

    if (enableCloth)
    {
        cloth->m_system_dt = deltaTime;
        cloth->Update();
        cloth->Draw(this->GetMainCamera());

        // reset if push the space key;
        if (Input::IsKeyPressed(Key::Space))
        {
            // cloth->Reset();
        }
    }

    // fluid
    if (enableFluid)
    {
        fluid->Update();
        fluid->Draw(this->GetMainCamera());
    }
}

} // namespace Rudy