#version 330 core
out vec4 FragColor;

in vec3 WorldCoords;

uniform samplerCube u_Skybox;  


void main()
{
    //test
    FragColor = texture(u_Skybox, WorldCoords);


    //FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}