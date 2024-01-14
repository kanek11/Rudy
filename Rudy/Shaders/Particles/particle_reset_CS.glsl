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


uniform uint u_maxParticleCount; 


layout(std430, binding = 0) buffer Counters_t
{
    uint dead_count;
    uint alive_count[2];
    uint emission_count;
    uint update_count;
}
Counters;

layout(std430, binding = 1) buffer ParticleDeadIndices_t
{
	uint indices[];
} 
DeadIndices;
 
 

layout(std430, binding = 2) buffer UpdateDispatchArgs_t
{
    uint num_groups_x;
    uint num_groups_y;
    uint num_groups_z;
}
UpdateDispatchArgs;


layout(std430, binding = 3) buffer EmissionDispatchArgs_t
{
    uint num_groups_x;
    uint num_groups_y;
    uint num_groups_z;
}
EmissionDispatchArgs;

 



// ------------------------------------------------------------------
// MAIN -------------------------------------------------------------
// ------------------------------------------------------------------

void main()
{
    uint index = gl_GlobalInvocationID.x;

    if (index == 0)
    {
        // Initialize counts
        Counters.dead_count = u_maxParticleCount;
        Counters.alive_count[0] = 0;
        Counters.alive_count[1] = 0;
        Counters.emission_count = 0;


        UpdateDispatchArgs.num_groups_x = 0;
        UpdateDispatchArgs.num_groups_y = 1;
        UpdateDispatchArgs.num_groups_z = 1; 
         

        EmissionDispatchArgs.num_groups_x = 0;
        EmissionDispatchArgs.num_groups_y = 1;
        EmissionDispatchArgs.num_groups_z = 1;
        
    }


    if (index < u_maxParticleCount)
        DeadIndices.indices[index] = index;


}