#version 460 core
out vec4 FragColor;

in vec3 WorldCoords;


uniform samplerCube u_SkyboxTexture;  
uniform float u_mipLevel;

void main()
{
    //test
    //FragColor = texture(u_SkyboxTexture, WorldCoords);
     FragColor = textureLod(u_SkyboxTexture, WorldCoords, u_mipLevel);

    //FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}