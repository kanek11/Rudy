#version 460 core
// ------------------------------------------------------------------
// CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------

#define PI 3.1415926535

// enum of field type
const int U    = 0;
const int V    = 1;
const int CELL = 2;

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

void main()
{
    uint i = gl_GlobalInvocationID.x;
    uint j = gl_GlobalInvocationID.y;

    uint height = u_height;
    uint width  = u_width;

    if (i > width - 1 || j > height - 1)
    {
        return;
    }

    uint center = i * height + j;

    vel_u.vel_u[center] = vel_u.vel_u[center] + vel_u_post.vel_u_post[center];
    vel_v.vel_v[center] = vel_v.vel_v[center] + vel_v_post.vel_v_post[center];

    // reset post values
    vel_u_post.vel_u_post[center] = 0.0f;
    vel_v_post.vel_v_post[center] = 0.0f;
}