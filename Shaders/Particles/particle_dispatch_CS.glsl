#version 460 core

// ------------------------------------------------------------------
// CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------
 

// ------------------------------------------------------------------
// INPUTS -----------------------------------------------------------
// ------------------------------------------------------------------

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

layout(std430, binding = 0) buffer Counters_t
{
	uint dead_count;
	uint alive_count[2];
	uint emission_count;
	uint update_count;
}
Counters;
 

layout(std430, binding = 1) buffer UpdateDispatchArgs_t
{
	uint num_groups_x;
	uint num_groups_y;
	uint num_groups_z;
}
UpdateDispatchArgs;


layout(std430, binding = 2) buffer EmissionDispatchArgs_t
{
	uint num_groups_x;
	uint num_groups_y;
	uint num_groups_z;
}
EmissionDispatchArgs;



uniform uint  u_local_size_x;

uniform uint u_emission_count;

uniform uint u_preSimIndex;
uniform uint u_postSimIndex; 


void main()
{
	if (gl_GlobalInvocationID.x == 0)   
	{
		uint emission_count = min(u_emission_count, Counters.dead_count);
		Counters.emission_count = emission_count;

		EmissionDispatchArgs.num_groups_x = uint(ceil(float(emission_count) / float(u_local_size_x)));
		EmissionDispatchArgs.num_groups_y = 1;
		EmissionDispatchArgs.num_groups_z = 1;


		uint update_count = Counters.alive_count[u_preSimIndex] + emission_count;
		Counters.update_count = update_count;

		UpdateDispatchArgs. num_groups_x = uint(ceil(float(update_count) / float(u_local_size_x)));
        UpdateDispatchArgs. num_groups_y = 1;
        UpdateDispatchArgs. num_groups_z = 1;

		Counters.alive_count[u_postSimIndex] = 0;
	
	}

}
