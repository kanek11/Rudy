#version 460 core 
out vec4 FragColor; 

in vec2 TexCoords;

uniform sampler2D u_ScreenQuadTexture;

uniform bool u_IsGrayScale;

void main()
{    
    if ( u_IsGrayScale)
    { 
        FragColor = vec4(vec3(texture(u_ScreenQuadTexture, TexCoords).r), 1.0);
	}
    else 
    {
        //FragColor = vec4(vec3(0.0, 1.0, 0.0), 1.0);

        FragColor = vec4(texture(u_ScreenQuadTexture, TexCoords).rgb, 1.0);
	}
		 
     
    
}