#pragma once
#include "RudyPCH.h"

#include "Rudy/Renderer/Object.h"
#include "Rudy/Renderer/Buffer.h"
#include "Rudy/Renderer/RendererComponent.h"

namespace Rudy
{

const uint32_t MAX_CONSTRAINTS = 16;

struct stretchConstIDs
{
    uint32_t IDs[MAX_CONSTRAINTS];
};

class Cloth
{
public:
    ~Cloth() = default;
    Cloth();

    void Init();
    void Update();
    void CPU_Update();
    void PBD_step();
    void ComputeStretchRestLengths();
    void ComputeBendRestLengths();
    void ComputeNormals();

    uint32_t m_num_subSteps       = 16;
    float    m_stretch_compliance = 0.0f;
    float    m_bend_compliance    = 3.0f;

    bool useLegacyShader = false;

    std::vector<glm::vec4> m_correction;

    // constraints
    std::vector<uint32_t> m_edgePairIDs;
    std::vector<uint32_t> m_triPairIDs;

    SharedPtr<StorageBuffer> m_stretchConstPair_IDs_buffer;
    SharedPtr<StorageBuffer> m_bendConstPair_IDs_buffer;

    std::vector<float>     m_restLength;
    std::vector<float>     m_invMass;
    std::vector<glm::vec4> m_positions;
    std::vector<glm::vec4> m_velocities;
    std::vector<glm::vec4> m_normals;

    uint32_t m_num_particles = 0;
    uint32_t m_num_triangles = 0;
    uint32_t m_num_edges     = 0;

    uint32_t m_num_edgePair = 0;
    uint32_t m_num_triPair  = 0;

    uint32_t m_num_stretchConst = 0;
    uint32_t m_num_bendConst    = 0;

    void Draw(SharedPtr<Camera> cam);

    // fornow:
public:
    void            SetMesh(SharedPtr<Mesh> m);
    SharedPtr<Mesh> GetMesh() { return m_mesh; }
    bool            hasMesh() { return m_mesh != nullptr; }

public:
    SharedPtr<Mesh>        m_mesh        = nullptr;
    SharedPtr<Material>    m_material    = nullptr;
    SharedPtr<VertexArray> m_vertexArray = nullptr;
    SharedPtr<IndexBuffer> m_indexBuffer = nullptr;

    // cloth parameters

    float m_gravity = -9.8f;

public:
    float m_system_dt        = 0.0f;
    float m_time_accumulator = 0.0f;
    float m_deltaTime        = 0.0f;

    float m_FPS              = 24.0f;
    float m_solver_deltaTime = 0.0f;

    uint32_t m_local_size_x = 32;
    uint32_t m_local_size_y = 1;
    uint32_t m_local_size_z = 1;

public:
    // buffers
    // particles attributes
    SharedPtr<StorageBuffer> m_particle_position_buffer;
    SharedPtr<StorageBuffer> m_particle_rest_position_buffer;
    SharedPtr<StorageBuffer> m_particle_previous_position_buffer;

    SharedPtr<StorageBuffer> m_particle_correction_buffer; // jacobi iteration

    SharedPtr<StorageBuffer> m_particle_velocity_buffer;
    SharedPtr<StorageBuffer> m_particle_inv_mass_buffer;

    // for rendering, vertex normals
    SharedPtr<StorageBuffer> m_particle_normal_buffer;
    SharedPtr<StorageBuffer> m_indices_WorldToView_buffer;

    SharedPtr<StorageBuffer> m_stretchConst_rest_length_buffer;
    SharedPtr<StorageBuffer> m_bendConst_rest_length_buffer;

    // kernels/compute shaders
    SharedPtr<Shader> m_prePBD_integrate_shader;
    SharedPtr<Shader> m_update_solve_stretchConst_shader;
    SharedPtr<Shader> m_update_solve_bendConst_shader;
    SharedPtr<Shader> m_postPBD_correction_shader;
    SharedPtr<Shader> m_postPBD_update_vel_shader;

    SharedPtr<Shader> m_compute_normals_shader;
    SharedPtr<Shader> m_normalize_normals_shader;

    SharedPtr<Shader> m_compute_rest_length_shader;

    // legacy use accumulations
    std::vector<stretchConstIDs> m_stretchConstIDs;
    std::vector<stretchConstIDs> m_bendConstIDs;

    SharedPtr<Shader> m_update_solve_stretchConst_legacy_shader;

    SharedPtr<StorageBuffer> m_stretchConst_IDs_buffer;
    SharedPtr<StorageBuffer> m_bendConst_IDs_buffer;
};

} // namespace Rudy