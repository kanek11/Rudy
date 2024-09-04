#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 WorldCoords;

uniform mat4 u_projection;
uniform mat4 u_view;
// make sure the view matrix reduce to mat3, no translation part

void main()
{
    WorldCoords = aPos;

    vec4 pos = u_projection * u_view * vec4(aPos, 1.0);
    // gl_Position = pos.xyzw; // normal cube;
    gl_Position = pos.xyww;
    // hack to make sure z is always 1;
    // st the skybox is always behind everything in the scene;
    // or change the depth test to GL_LEQUAL in the host: glDepthFunc(GL_LEQUAL);
}