#version 330 core
out vec4 FragColor;


in vec2 TexCoords;
uniform sampler2D gAlbedo;


void main()
{
    //test
    FragColor = texture(gAlbedo, TexCoords); 
}