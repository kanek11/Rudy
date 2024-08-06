#include "RudyPCH.h"

#include "Fluid.h"

namespace Rudy
{

Fluid::Fluid() :
    RenderableObject()
{
    RD_CORE_INFO("Fluid: grid size: {0} x {1}", m_num_width, m_num_height);
};

void Fluid::Draw(SharedPtr<Camera> cam)
{
    m_screen_quad->Draw(nullptr);
}

void Fluid::Init()
{
    // create buffers
    uint32_t buffer_size_float = sizeof(float) * m_num_cells;
    uint32_t buffer_size_int   = sizeof(int) * m_num_cells;

    m_vel_u      = StorageBuffer::Create();
    m_vel_v      = StorageBuffer::Create();
    m_vel_u_post = StorageBuffer::Create();
    m_vel_v_post = StorageBuffer::Create();
    m_boundary   = StorageBuffer::Create();
    m_mass       = StorageBuffer::Create();
    m_mass_post  = StorageBuffer::Create();

    // initial data;  column major index;
    std::vector<float> vel_u_data(m_num_cells, 0.0f);
    std::vector<float> vel_v_data(m_num_cells, 0.0f);

    std::vector<int>   boundary(m_num_cells, 1);
    std::vector<float> mass_data(m_num_cells, 1.0f);

    // first column as solid boundary; last as free boundary
    for (int j = 0; j < m_num_height; j++)
    {
        boundary[0 + j] = 0;
        // boundary[(m_num_width - 1) * m_num_height + j] = 2;
    }

    // top, bottom row as solid boundary
    for (int i = 0; i < m_num_width; i++)
    {
        boundary[i * m_num_height + 0]                = 0;
        boundary[i * m_num_height + m_num_height - 1] = 0;
    }

    // set 2nd column of u velocity;
    for (int j = 0; j < m_num_height; j++)
    {
        vel_u_data[1 * m_num_height + j] = 2.0f;
    }

    // vortex shedding: set middle subset of 1st column as low mass/density;
    float windPipeHeight = 0.1f * m_num_height;

    int start = ceil(m_num_height / 2.0) - ceil(windPipeHeight / 2.0);
    int end   = floor(m_num_height / 2.0) + floor(windPipeHeight / 2.0);

    for (int j = start; j < end; j++)
    {
        mass_data[j + 0] = 0.0f;
    }

    //================================================================================================
    // set a obstacle sphere near the outlet
    // manual voxelization
    float     radius = 0.15f;
    glm::vec2 center = glm::vec2(0.4f, 0.5f);

    int start_i = std::max(0, static_cast<int>((center.x - radius) / m_dx));
    int end_i   = std::min(int(m_num_width), static_cast<int>((center.x + radius) / m_dx) + 1);
    int start_j = std::max(0, static_cast<int>((center.y - radius) / m_dx));
    int end_j   = std::min(int(m_num_height), static_cast<int>((center.y + radius) / m_dx) + 1);

    for (int i = start_i; i < end_i; i++)
    {
        for (int j = start_j; j < end_j; j++)
        {
            // physical position of the cell
            float x = (i + 0.5) * m_dx;
            float y = (j + 0.5) * m_dx;

            if (glm::distance(glm::vec2(x, y), center) < radius)
            {
                boundary[i * m_num_height + j] = 0;
            }
        }
    }

    // upload data
    m_vel_u->SetData(vel_u_data.data(), buffer_size_float);
    m_vel_v->SetData(vel_v_data.data(), buffer_size_float);
    m_vel_u_post->SetData(vel_u_data.data(), buffer_size_float);
    m_vel_v_post->SetData(vel_v_data.data(), buffer_size_float);
    m_boundary->SetData(boundary.data(), buffer_size_int);
    m_mass->SetData(mass_data.data(), buffer_size_float);
    m_mass_post->SetData(mass_data.data(), buffer_size_float);

    // shaders
    m_advection_shader      = Shader::CreateComputeShader("fluid advect CS", "Shaders/Fluid/fluid_advection_CS.glsl");
    m_advection_attr_shader = Shader::CreateComputeShader("fluid advect attr CS", "Shaders/Fluid/fluid_advection_attr_CS.glsl");

    m_projection_shader = Shader::CreateComputeShader("fluid project CS", "Shaders/Fluid/fluid_projection_CS.glsl");

    m_colormap_shader = Shader::CreateComputeShader("fluid colormap CS", "Shaders/Fluid/fluid_colormap_CS.glsl");

    m_post_projection_shader = Shader::CreateComputeShader("fluid post proj CS", "Shaders/Fluid/fluid_post_projection_CS.glsl");

    // render quad for output
    m_screen_quad = ScreenQuad::Create();

    m_output_texture = Texture2D::CreateEmpty(
        TextureSpec { m_tex_width, m_tex_height, TextureInternalFormat::RGBA32F, false, WrapMode::Repeat, FilterMode::Linear, FilterMode::Linear });

    m_output_shader = Shader::Create("fluid output shader",
                                     "Shaders/Shaders/ScreenQuad_VS.glsl",
                                     "Shaders/Shaders/ScreenQuad_FS.glsl");

    m_output_material = Material::Create(m_output_shader);
    m_screen_quad->SetMaterial(m_output_material);
    m_output_material->SetTexture(TexType::ScreenTexture, m_output_texture);
}

void Fluid::Update()
{
    // benchmark
    RD_PROFILE_FUNCTION();

    // projection
    if (true)
    {
        if (CPU_Proj)
        {
            // map data£»
            float* _vel_u      = (float*)glMapNamedBuffer(m_vel_u->GetBufferID(), GL_READ_WRITE);
            float* _vel_v      = (float*)glMapNamedBuffer(m_vel_v->GetBufferID(), GL_READ_WRITE);
            float* _vel_u_post = (float*)glMapNamedBuffer(m_vel_u_post->GetBufferID(), GL_READ_WRITE);
            float* _vel_v_post = (float*)glMapNamedBuffer(m_vel_v_post->GetBufferID(), GL_READ_WRITE);
            int*   _boundary   = (int*)glMapNamedBuffer(m_boundary->GetBufferID(), GL_READ_WRITE);

            for (int k = 0; k < m_num_iter; k++)
            {
                for (int i = 1; i < m_num_width - 1; i++)
                {
                    for (int j = 1; j < m_num_height - 1; j++)
                    {
                        int center = i * m_num_height + j;
                        int n      = m_num_height;

                        if (_boundary[center] == 0)
                        {
                            continue;
                        }

                        float sx0 = static_cast<float>(_boundary[(i - 1) * n + j]);
                        float sx1 = static_cast<float>(_boundary[(i + 1) * n + j]);
                        float sy0 = static_cast<float>(_boundary[i * n + j - 1]);
                        float sy1 = static_cast<float>(_boundary[i * n + j + 1]);

                        float s = sx0 + sx1 + sy0 + sy1;

                        float u_left  = _vel_u[center];
                        float u_right = _vel_u[(i + 1) * n + j];

                        float v_down = _vel_v[center];
                        float v_up   = _vel_v[i * n + j + 1];

                        float div = (u_right - u_left) + (v_up - v_down);

                        float p = -div / s;
                        p *= m_over_relaxation;

                        _vel_u[i * n + j] -= sx0 * p;
                        _vel_u[(i + 1) * n + j] += sx1 * p;
                        _vel_v[i * n + j] -= sy0 * p;
                        _vel_v[i * n + j + 1] += sy1 * p;
                    }
                }
            }

            glUnmapNamedBuffer(m_vel_u->GetBufferID());
            glUnmapNamedBuffer(m_vel_v->GetBufferID());
            glUnmapNamedBuffer(m_vel_u_post->GetBufferID());
            glUnmapNamedBuffer(m_vel_v_post->GetBufferID());
            glUnmapNamedBuffer(m_boundary->GetBufferID());

            // for safety:
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
        else
        {
            for (int i = 0; i < m_num_iter; i++)
            {
                if (true)
                {
                    // iteration
                    m_projection_shader->Bind();

                    // uniforms
                    m_projection_shader->SetUInt("u_width", m_num_width);
                    m_projection_shader->SetUInt("u_height", m_num_height);
                    m_projection_shader->SetFloat("u_dx", m_dx);
                    m_projection_shader->SetFloat("u_dt", m_dt);
                    m_projection_shader->SetFloat("u_over_relaxation", m_over_relaxation);

                    m_vel_u->BindBase(0);
                    m_vel_v->BindBase(1);
                    m_vel_u_post->BindBase(2);
                    m_vel_v_post->BindBase(3);
                    m_boundary->BindBase(4);
                    m_mass->BindBase(5);
                    m_mass_post->BindBase(6);

                    glDispatchCompute(glm::ceil(float(m_num_width) / float(m_local_size_x)), glm::ceil(float(m_num_height) / m_local_size_y), 1);
                    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
                    glMemoryBarrier(GL_ALL_BARRIER_BITS);

                    m_projection_shader->Unbind();
                }
                if (true)
                {
                    m_post_projection_shader->Bind();
                    m_post_projection_shader->SetUInt("u_width", m_num_width);
                    m_post_projection_shader->SetUInt("u_height", m_num_height);
                    m_post_projection_shader->SetFloat("u_dx", m_dx);
                    m_post_projection_shader->SetFloat("u_dt", m_dt);
                    m_post_projection_shader->SetFloat("u_over_relaxation", m_over_relaxation);

                    m_vel_u->BindBase(0);
                    m_vel_v->BindBase(1);
                    m_vel_u_post->BindBase(2);
                    m_vel_v_post->BindBase(3);
                    m_boundary->BindBase(4);
                    m_mass->BindBase(5);
                    m_mass_post->BindBase(6);

                    glDispatchCompute(glm::ceil(float(m_num_width) / float(m_local_size_x)), glm::ceil(float(m_num_height) / m_local_size_y), 1);
                    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
                    glMemoryBarrier(GL_ALL_BARRIER_BITS);

                    m_post_projection_shader->Unbind();
                }
            }
        }
    }

    // advection
    if (true)
    {
        m_advection_shader->Bind();

        // uniforms
        m_advection_shader->SetUInt("u_width", m_num_width);
        m_advection_shader->SetUInt("u_height", m_num_height);

        m_advection_shader->SetFloat("u_dx", m_dx);
        m_advection_shader->SetFloat("u_dt", m_dt);
        m_advection_shader->SetFloat("u_over_relaxation", m_over_relaxation);

        m_vel_u->BindBase(0);
        m_vel_v->BindBase(1);
        m_vel_u_post->BindBase(2);
        m_vel_v_post->BindBase(3);
        m_boundary->BindBase(4);
        m_mass->BindBase(5);
        m_mass_post->BindBase(6);

        glDispatchCompute(glm::ceil(float(m_num_width) / float(m_local_size_x)), glm::ceil(float(m_num_height) / m_local_size_y), 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_advection_shader->Unbind();
    }

    if (true)
    {
        m_advection_attr_shader->Bind();
        m_advection_attr_shader->SetUInt("u_width", m_num_width);
        m_advection_attr_shader->SetUInt("u_height", m_num_height);
        m_advection_attr_shader->SetFloat("u_dx", m_dx);
        m_advection_attr_shader->SetFloat("u_dt", m_dt);
        m_advection_attr_shader->SetFloat("u_over_relaxation", m_over_relaxation);

        m_vel_u->BindBase(0);
        m_vel_v->BindBase(1);
        m_vel_u_post->BindBase(2);
        m_vel_v_post->BindBase(3);
        m_boundary->BindBase(4);
        m_mass->BindBase(5);
        m_mass_post->BindBase(6);

        glDispatchCompute(glm::ceil(float(m_num_width) / float(m_local_size_x)), glm::ceil(float(m_num_height) / m_local_size_y), 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_advection_attr_shader->Unbind();
    }

    // render
    if (true)
    {
        m_colormap_shader->Bind();

        m_colormap_shader->SetUInt("u_width", m_num_width);
        m_colormap_shader->SetUInt("u_height", m_num_height);

        m_vel_u->BindBase(0);
        m_vel_v->BindBase(1);
        m_vel_u_post->BindBase(2);
        m_vel_v_post->BindBase(3);
        m_boundary->BindBase(4);
        m_mass->BindBase(5);
        m_mass_post->BindBase(6);

        glBindImageTexture(0, m_output_texture->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        glDispatchCompute(glm::ceil(float(m_num_width) / float(m_local_size_x)), glm::ceil(float(m_num_height) / m_local_size_y), 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        m_colormap_shader->Unbind();
    }
}

} // namespace Rudy