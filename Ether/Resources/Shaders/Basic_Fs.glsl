#version 330 core

out vec4 FragColor;

uniform vec3 u_Color;

void main()
{
    if (u_Color == vec3(0.0f))
		FragColor = vec4(1.0f,1.0f,1.0f,1.0f);   
	else
        FragColor = vec4(u_Color,1.0f);

}