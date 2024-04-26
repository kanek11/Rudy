#version 460 core
#extension GL_NV_shader_atomic_float : require

// ------------------------------------------------------------------
// CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------

#define PI 3.1415926535

// ------------------------------------------------------------------
// INPUTS -----------------------------------------------------------
// ------------------------------------------------------------------

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

uniform uint  u_height;
uniform uint  u_width;
uniform float u_dx;
uniform float u_dt;
uniform float u_over_relaxation;

// ------------------------------------------------------------------
// BUFFERS ---------------------------------------------------------
// ------------------------------------------------------------------

layout(std430, binding = 0) buffer vel_u_t
{
    float vel_u[];
}
vel_u;

layout(std430, binding = 1) buffer vel_v_t
{
    float vel_v[];
}
vel_v;

layout(std430, binding = 2) buffer vel_u_post_t
{
    float vel_u_post[];
}
vel_u_post;

layout(std430, binding = 3) buffer vel_v_post_t
{
    float vel_v_post[];
}
vel_v_post;

layout(std430, binding = 4) buffer boundary_t
{
    int boundary[];
}
boundary;

layout(std430, binding = 5) buffer mass_t
{
    float mass[];
}
mass;

layout(std430, binding = 6) buffer mass_post_t
{
    float mass_post[];
}
mass_post;

// ------------------------------------------------------------------
// Images ---------------------------------------------------------
// ------------------------------------------------------------------

// layout(binding = 0, rgba32f) uniform image2D image0;

// impose zero divergence
void main()
{
    // common
    uint i = gl_GlobalInvocationID.x;
    uint j = gl_GlobalInvocationID.y;

    uint height = u_height;
    uint width  = u_width;

    if (i > width - 1 || j > height - 1)
    {
        return;
    }

    uint center = i * height + j;

    // skip boundary
    if (i == 0 || i == width - 1 || j == 0 || j == height - 1)
    {
        return;
    }

    // skip inner boundary cells
    if (boundary.boundary[center] != 1)
    {
        return;
    }

    uint left  = (i - 1) * height + j;
    uint right = (i + 1) * height + j;

    uint bottom = i * height + j - 1;
    uint top    = i * height + j + 1;

    float sx0 = float(boundary.boundary[left]);
    float sx1 = float(boundary.boundary[right]);
    float sy0 = float(boundary.boundary[bottom]);
    float sy1 = float(boundary.boundary[top]);

    float solid_count = sx0 + sx1 + sy0 + sy1;
    if (solid_count == 0.0f)
    {
        return; // surrounded by solid
    }

    // diveragne for staggered grid ; -1/2 + 1/2=0, +1/2 + 1/2=1
    float div = vel_u.vel_u[right] - vel_u.vel_u[center]
        + vel_v.vel_v[top] - vel_v.vel_v[center];

    float temp = -div / solid_count;
    temp *= u_over_relaxation;

    // update velocity by atomatic add
    atomicAdd(vel_u_post.vel_u_post[right], +sx1 * temp);
    atomicAdd(vel_u_post.vel_u_post[center], -sx0 * temp);
    atomicAdd(vel_v_post.vel_v_post[top], +sy1 * temp);
    atomicAdd(vel_v_post.vel_v_post[center], -sy0 * temp);
}
