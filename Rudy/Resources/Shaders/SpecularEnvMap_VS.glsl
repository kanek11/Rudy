#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 WorldPos;

uniform mat4 u_ProjectionView;

void main()
{
    WorldPos = aPos;
    gl_Position = u_ProjectionView * vec4(WorldPos, 1.0);
}