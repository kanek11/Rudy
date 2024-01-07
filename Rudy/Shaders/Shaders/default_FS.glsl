#version 460 core

layout(location = 0) out vec4 out_FragColor;

layout(location = 0) in vec3 v_normal;


//uniform vec3 u_Color;
const vec3 inv_light_dir = vec3(0.3, 0.6, 0.9);

void main()
{ 
	float d = dot(normalize(v_normal), normalize(inv_light_dir));
	float coeff = max(0.0, d);


    //dark pink suggests lack of color
	out_FragColor = vec4( coeff * vec3 (1.0f, 0.1f, 1.0f), 1.0f);

}