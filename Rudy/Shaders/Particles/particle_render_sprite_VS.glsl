#version 460 core 

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


 
void main()
{ 
	uint index = AliveIndicesPostSim.indices[gl_InstanceID];

	vec3 particle_offset = ParticlePositions.positions[index];

	gl_Position = vec4(particle_offset, 1.0);
 
}