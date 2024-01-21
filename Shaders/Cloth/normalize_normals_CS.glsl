#version 460 core
#extension GL_NV_shader_atomic_float : require

// ------------------------------------------------------------------
// CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------

#define PI 3.1415926535

// ------------------------------------------------------------------
// INPUTS -----------------------------------------------------------
// ------------------------------------------------------------------

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

uniform uint u_num_vertices;


// ------------------------------------------------------------------
// BUFFERS ---------------------------------------------------------
// ------------------------------------------------------------------

 
layout(std430, binding = 0) buffer Normals_t
{
    vec4 normals[];

}Normals;

 

 
void main()
{
    uint index = gl_GlobalInvocationID.x;
    if (index >= u_num_vertices) return;

    vec3 normal = normalize(Normals.normals[index].xyz); 
    Normals.normals[index] = vec4(normal, 0.0); 

}