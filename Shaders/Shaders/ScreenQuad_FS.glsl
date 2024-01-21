#version 460 core 
out vec4 FragColor; 

in vec2 TexCoords;

layout(location = 0)uniform sampler2D u_ScreenTexture;

uniform bool u_IsGrayScale;
uniform float u_mipLevel;

void main()
{    
    if (u_IsGrayScale)
    { 
        FragColor = vec4(vec3(textureLod(u_ScreenTexture, TexCoords, u_mipLevel).r), 1.0);
	}
    else 
    { 
        FragColor = vec4(textureLod(u_ScreenTexture, TexCoords, u_mipLevel).rgb, 1.0);
	}     
}