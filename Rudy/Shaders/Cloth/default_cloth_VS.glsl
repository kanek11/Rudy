#version 460 core
layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec4 in_normal;

out VS_OUT
{
    vec3 world_position;
    vec3 normal;

} vs_out;


uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;


void main()
{ 
    vec3 Normal =  normalize(in_normal.xyz); 
    vs_out.normal = (u_model * vec4(Normal, 0.0)).xyz;

    vec3 WorldPos = vec3(u_model * vec4(in_pos.xyz, 1.0));
    vs_out.world_position = WorldPos;

    gl_Position = u_projection * u_view * vec4(WorldPos, 1.0);



}