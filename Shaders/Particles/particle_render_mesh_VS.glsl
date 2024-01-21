#version 460 core 
/* Mesh data */
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec3 in_normal; 

layout(location = 0) out vec3 v_normal;

layout(std430, binding = 0) buffer ParticleIndices_t
{ 
	uint indices[];
}
AliveIndicesPostSim;


layout(std430, binding = 1) buffer ParticlePosition_t
{
	vec3 positions[];
}
ParticlePositions; 

  

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{

	uint index =  AliveIndicesPostSim.indices[gl_InstanceID];

	vec3 particle_offset = ParticlePositions.positions[index];

	vec3 position_offset = in_position + particle_offset;
	 
	v_normal = (u_model * vec4(in_normal, 0.0)).xyz; 

	//hard code scale
	float scale = 0.1f;
	vec3 WorldPos = scale * vec3(u_model * vec4(position_offset, 1.0));
	gl_Position = u_projection* u_view * vec4(WorldPos, 1.0); 


}