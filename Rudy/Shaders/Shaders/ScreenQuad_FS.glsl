#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

// glBindTextureUnit(0, texture->GetID());
layout(location = 0) uniform sampler2D u_ScreenTexture;

uniform bool u_isGrayScale;

uniform bool u_isDepthTexture;

uniform float u_mipLevel;

uniform float u_near_plane;
uniform float u_far_plane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float zValue = depth * 2.0 - 1.0; // Back to NDC
    float near   = u_near_plane;
    float far    = u_far_plane;
    return (2.0 * near * far) / (far + near - zValue * (far - near));
}

void main()
{
    if (u_isGrayScale)
    {
        FragColor = vec4(vec3(textureLod(u_ScreenTexture, TexCoords, u_mipLevel).r), 1.0);
    }
    else if (u_isDepthTexture)
    {
        float depthValue = textureLod(u_ScreenTexture, TexCoords, u_mipLevel).r;
        FragColor        = vec4(vec3(LinearizeDepth(depthValue) / u_far_plane), 1.0); // perspective
                                                                                      // FragColor = vec4(vec3(depthValue), 1.0); // orthographic
    }
    else
    {
        FragColor = vec4(textureLod(u_ScreenTexture, TexCoords, u_mipLevel).rgb, 1.0);
    }
}
