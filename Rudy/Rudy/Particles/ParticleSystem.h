#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Buffer.h"
#include "Rudy/Renderer/Object.h"

namespace Rudy
{

// structs that specify layout, also for GPU buffers

// warning: somehow the std140 and std430 layout have different alignment rules
// for std430, the next multiple of 16 bytes ,means vec3 is really 16 bytes ,
// always use vec4  or vec3 + padding;
struct Particle
{
    glm::vec4 position;
    glm::vec4 velocity;

    float lifeTime;
    float age;
};

struct Counter
{
    uint32_t deadCount;
    uint32_t aliveCount[2]; // pre-sim and post-sim
    uint32_t emissionCount; // depend on dynamic , avaliable #dead particles
    uint32_t updateCount;   // depend on dynamic , avaliable #alive particles
};

struct DispatchComputeIndirectCommand
{
    uint32_t numGroupsX = 1;
    uint32_t numGroupsY = 1;
    uint32_t numGroupsZ = 1;
};

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();

    void Render();
};

// void OnUpdate(float deltaTime);
// void OnRender();
// void OnImGuiRender();

class Emitter : public RenderableObject
{
public:
    ~Emitter();
    Emitter();

    static SharedPtr<Emitter> Create();
    void                      InitComponent(SharedPtr<Emitter> object);

    // basic stages
    void Spawn();
    void Update();
    void Reset();

    // inherit rendering method;
    void Draw(SharedPtr<Camera> cam) override;

public:
    // system parameters
    float m_deltaTime = 0.0f;

    uint32_t m_local_size_x = 32;
    uint32_t m_local_size_y = 1;
    uint32_t m_local_size_z = 1;

    uint32_t m_maxParticleCount = 1024;

    uint32_t m_currentAliveCount = 0;

    float m_emissionAccumulator = 0.0f;

    // debug info
    uint32_t totalEmissionCount = 0;
    float    EmissionTime       = 0.0f;

    // emitter spawn

    glm::vec3 m_emitter_position = { 0.0f, 10.0f, 0.0f };

    float m_emission_rate = 32;

    // emitter shape
    int m_emission_shape = 0;
    // sphere
    float m_sphereRadius = 1.0f;

    // cone
    glm::vec3 m_cone_axis_direction = { 0.0f, 1.0f, 0.0f };
    float     m_cone_angle          = 45.0f;

    // emitter update
    uint32_t m_preSimIndex  = 0;
    uint32_t m_postSimIndex = 1;

    // particle spawn
    float m_particle_minLifetime     = 3.0f;
    float m_particle_maxLifetime     = 10.0f;
    float m_particle_minInitialSpeed = 5.0f;
    float m_particle_maxInitialSpeed = 10.0f;

    // particle simulation/update

public:
    // system resources
    //======shader programs======//
    //  each shader is a stage in the stack;
    SharedPtr<Shader> m_particle_reset_compute_shader;
    SharedPtr<Shader> m_particle_dispatch_compute_shader;
    SharedPtr<Shader> m_particle_emission_compute_shader;
    SharedPtr<Shader> m_particle_update_compute_shader;

    //======buffers======//

    // particles attributes
    SharedPtr<StorageBuffer> m_particle_position_buffer;
    SharedPtr<StorageBuffer> m_particle_velocity_buffer;
    SharedPtr<StorageBuffer> m_particle_age_buffer;
    SharedPtr<StorageBuffer> m_particle_lifeTime_buffer;

    // management of alive/dead particles
    SharedPtr<StorageBuffer> m_aliveList_buffer[2];
    SharedPtr<StorageBuffer> m_deadList_buffer;
    SharedPtr<StorageBuffer> m_counter_buffer;

    // indirect for dispatch of each stage ; for dynamic length updated by compute
    // shader
    SharedPtr<StorageBuffer> m_indirect_dispatch_update_buffer;
    SharedPtr<StorageBuffer> m_indirect_dispatch_emission_buffer;

    // rendering

    SharedPtr<Texture2D> radial_sprite = nullptr;

    SharedPtr<Shader> m_particle_sprite_shader = nullptr;
    SharedPtr<Shader> m_particle_mesh_shader   = nullptr;
    enum class RENDER_TYPE
    {
        POINTS = 0,
        SPRITES,
        MESHES,
    };

    RENDER_TYPE m_render_type = RENDER_TYPE::SPRITES;
};

// float rotation = 0.0f;
// float lifeRemaining = 0.0f;
// bool active = false;
// glm::vec4 colorBegin, colorEnd;
// float sizeBegin, sizeEnd;

} // namespace Rudy