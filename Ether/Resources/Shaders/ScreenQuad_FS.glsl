#version 330 core 
out vec4 FragColor; 

in vec2 TexCoords;

uniform sampler2D ScreenQuad;

void main()
{  
    //if gray scale, just use r channel
    vec3 SampleColor =  texture(ScreenQuad, TexCoords).rgb;

    if (SampleColor.g == 0.0 && SampleColor.b == 0.0 )
    {
		FragColor = vec4(vec3(texture(ScreenQuad, TexCoords).r), 1.0);
	}
    else
    {
		FragColor = vec4(texture(ScreenQuad, TexCoords).rgb, 1.0);
	}
		 
     
   
    //FragColor = vec4(vec3(0.0,1.0,0.0), 1.0);
}