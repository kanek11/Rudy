#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 WorldFragPos;

uniform mat4 u_ProjectionView;

void main()
{
    WorldFragPos = aPos;
    gl_Position = u_ProjectionView * vec4(WorldFragPos, 1.0);
}