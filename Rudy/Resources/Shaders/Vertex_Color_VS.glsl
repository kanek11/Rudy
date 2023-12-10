#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor; 

uniform mat4 u_ProjectionView;
uniform mat4 u_Model; 
out vec3 Color;

void main()
{
    vec3 WorldPos = vec3(u_Model * vec4(aPos, 1.0));
    gl_Position = u_ProjectionView * vec4(WorldPos, 1.0);
    Color = aColor;
}
 
 