#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor; 

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model; 
out vec3 Color;

void main()
{
    vec3 WorldPos = vec3(u_model * vec4(aPos, 1.0));
    gl_Position = u_projection * u_view * vec4(WorldPos, 1.0);
    Color = aColor;
}
 
 