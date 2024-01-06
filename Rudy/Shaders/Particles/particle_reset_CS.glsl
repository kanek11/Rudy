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


uniform int u_maxParticleCount;



layout(std430, binding = 0) buffer ParticleDeadIndices_t
{
	uint indices[];
} 
DeadIndices;

layout(std430, binding = 1) buffer Counters_t
{
    uint dead_count;
    uint alive_count[2]; 
}
Counters;


layout(std430, binding = 2) buffer ParticleUpdateArgs_t
{
    uint numGroupsX;
    uint numGroupsY;
    uint numGroupsZ;
}
ParticleUpdateArgs;


layout(std430, binding = 3) buffer ParticleRenderArgs_t
{
    uint count;
    uint instance_count;
    uint first;
    uint base_instance;
}
ParticleRenderArgs;


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




        ParticleUpdateArgs.numGroupsX = 0;
        ParticleUpdateArgs.numGroupsY = 1;
        ParticleUpdateArgs.numGroupsZ = 1;


         
        //reset instance_count
        ParticleUpdateArgs.numGroupsX = 0;

        ParticleRenderArgs.count = 1;
        ParticleRenderArgs.instance_count = 0; 


        
    }

    if (index < u_maxParticleCount)
        DeadIndices.indices[index] = index;


}