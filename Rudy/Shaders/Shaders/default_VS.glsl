#version 460 core
layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec3 in_normal;

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
    vs_out.normal = (u_model * vec4(in_normal, 0.0)).xyz;
     
    vec3 WorldPos = vec3(u_model * vec4(in_Pos, 1.0));
    vs_out.world_position = WorldPos;

    gl_Position = u_projection * u_view * vec4(WorldPos, 1.0); 



}