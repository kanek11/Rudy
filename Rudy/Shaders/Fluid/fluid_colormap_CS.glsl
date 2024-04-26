#version 460 core

// ------------------------------------------------------------------
// INPUTS -----------------------------------------------------------
// ------------------------------------------------------------------

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

uniform uint u_height;
uniform uint u_width;

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

layout(rgba32f, binding = 0) uniform writeonly image2D dest_image;

void main()
{
    // texture index
    uint i = gl_GlobalInvocationID.x;
    uint j = gl_GlobalInvocationID.y;

    uint width  = u_width;
    uint height = u_height;

    if (i > width - 1 || j > height - 1)
    {
        return;
    }

    // stage: post_advection
    uint center         = i * height + j;
    vel_u.vel_u[center] = vel_u_post.vel_u_post[center];
    vel_v.vel_v[center] = vel_v_post.vel_v_post[center];
    mass.mass[center]   = mass_post.mass_post[center];

    // reset values
    vel_u_post.vel_u_post[center] = 0.0;
    vel_v_post.vel_v_post[center] = 0.0;
    mass_post.mass_post[center]   = 0.0;

    // colormapping
    vec4 color = vec4(vec3(0.0), 1.0);

    if (boundary.boundary[center] == 0)
    {
        // dark yellow
        color = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else if (boundary.boundary[center] == 1)
    {
        float density = mass.mass[i * height + j];

        // debug:
        // density = vel_u.vel_u[center];
        // density = vel_v.vel_v[center];
        // density    = boundary.boundary[center];
        color = vec4(vec3(density), 1.0);
    }

    // for data array,
    // column-major , left-bottom corner is (0,0)
    // for image, left-top corner is (0,0)
    uint img_i = i;
    uint img_j = height - j - 1;

    imageStore(dest_image, ivec2(img_i, img_j), color);
}
