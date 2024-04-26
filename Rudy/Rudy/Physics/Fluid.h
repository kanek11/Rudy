#pragma once
#include "RudyPCH.h"

#include "Rudy/Renderer/Object.h"
#include "Apps/Application.h"

namespace Rudy
{

// Eulerian fluid simulation
class Fluid : public RenderableObject
{
public:
    ~Fluid() = default;
    Fluid();

    // inherit rendering method;
    void Draw(Ref<Camera> cam) override;

    void Init();
    void Update();

public:
    // system parameters
    uint32_t m_local_size_x = 16;
    uint32_t m_local_size_y = 16;
    uint32_t m_local_size_z = 1;

    // fps
    uint32_t m_fps = 60;
    float    m_dt  = 1.0f / float(m_fps);

    float    m_over_relaxation = 1.9;
    uint32_t m_num_iter        = 100;

    // hardcode 1:2 ratio;  square cells;
    // in theory, it's possible to have non-square cells

    // dicretization resolution
    float m_aspect_ratio = Application::GetInstance()->m_viewportLayer->GetAspectRatio();

    // manually set grid size
    uint32_t m_grid_size_height = 100;
    uint32_t m_grid_size_width  = m_grid_size_height * m_aspect_ratio;

    // physical dimensions
    float m_domain_height = 1.0f;
    float m_domain_width  = m_domain_height * m_aspect_ratio;

    // spatial step;  square cells
    float m_dx = m_domain_height / m_grid_size_height;

    // dimension cell count
    uint32_t m_num_height = floor(m_domain_height / m_dx);
    uint32_t m_num_width  = floor(m_domain_width / m_dx);

    uint32_t m_num_cells = m_num_width * m_num_height;

    uint32_t m_tex_height = m_num_height;
    uint32_t m_tex_width  = m_num_width;

    // system resources
public:
    // buffers
    Ref<StorageBuffer> m_vel_u;
    Ref<StorageBuffer> m_vel_v;
    Ref<StorageBuffer> m_vel_u_post;
    Ref<StorageBuffer> m_vel_v_post;

    Ref<StorageBuffer> m_boundary;
    Ref<StorageBuffer> m_mass;
    Ref<StorageBuffer> m_mass_post;

    // shader
    Ref<Shader> m_advection_shader;
    Ref<Shader> m_advection_attr_shader;
    Ref<Shader> m_projection_shader;

    Ref<Shader> m_colormap_shader;

    Ref<Shader> m_post_projection_shader;

    // output texture and quad
    Ref<Shader>    m_output_shader;
    Ref<Texture2D> m_output_texture;
    Ref<Material>  m_output_material;

    Ref<ScreenQuad> m_screen_quad;

    bool CPU_Proj = true;

    int iter_temp = 100;
};
} // namespace Rudy
