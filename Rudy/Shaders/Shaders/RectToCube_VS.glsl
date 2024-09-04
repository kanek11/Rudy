#version 460 core
layout(location = 0) in vec3 aPos;

out vec4 FragPos;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    FragPos     = vec4(aPos, 1.0);
    gl_Position = u_projection * u_view * vec4(aPos, 1.0);
}