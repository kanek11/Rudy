#version 460 core

layout(location = 0) out vec4 out_FragColor;

in VS_OUT
{
	vec3 world_position;
	vec3 normal;

} fs_in;


//uniform vec3 u_Color;
const vec3 inv_light_dir = vec3(0.3, 0.6, 0.9);

void main()
{
	float LdotN = dot(normalize(fs_in.normal), normalize(inv_light_dir));
	float diff_coeff = max(0.0, LdotN);

	//dark pink suggests lack of color
	out_FragColor = vec4( diff_coeff *vec3(1.0f, 0.1f, 1.0f), 1.0f);


}