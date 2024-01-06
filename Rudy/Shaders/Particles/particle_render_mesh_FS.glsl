#version 460 core

layout(location = 0) out vec4 out_Frag_color;

layout(location = 0) in vec3 v_normal;

const vec3 light_dir = vec3(0.3, 0.6, 0.9);

uniform vec3 u_diffuse;

void main()
{
    float d = dot(normalize(v_normal), normalize(light_dir)); 
    float coeff = max(0.0, d);

    //debug: no lighting;
    //out_Frag_color = vec4(0.8,0.0,0.8, 1.0); 
    out_Frag_color = vec4( coeff * vec3(1.8, 0.0, 1.8), 1.0);
}