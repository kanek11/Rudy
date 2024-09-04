#version 460 core
out vec4 FragColor;

in vec3 WorldCoords;

uniform samplerCube u_SkyboxTexture;
uniform float       u_mipLevel;

void main()
{
    // test
    vec3 color = texture(u_SkyboxTexture, WorldCoords).rgb;

    color     = textureLod(u_SkyboxTexture, WorldCoords, u_mipLevel).rgb;
    FragColor = vec4(color, 1.0f);
    // FragColor = vec4(WorldCoords, 1.0f); // debug
}