#version 460 core

out vec4 FragColor;


uniform bool u_use_color;
uniform vec3 u_color;

void main()
{
	if (u_use_color)
		FragColor = vec4(u_color, 1.0f);
	else
	    FragColor = vec4(vec3(0.5f), 1.0f); 

}