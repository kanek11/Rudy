#version 460 core

// ------------------------------------------------------------------
// CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------

#define EMISSION_SHAPE_SPHERE 0

#define VELOCITY_MODE_FROM_POINT 0

#define PI 3.1415926535

// ------------------------------------------------------------------
// INPUTS -----------------------------------------------------------
// ------------------------------------------------------------------

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

 
//====system 
uniform vec3 u_seeds; 
//===emitter
//emitter spawn 
uniform vec3  u_emitter_position;

//uniform int   u_Emission_Shape;
//uniform int   u_velocity_Mode;

uniform float u_sphereRadius;

//emitter update
uniform uint u_preSimIndex; 


//======particle
//particle spawn
uniform float u_particle_minLifetime;
uniform float u_particle_maxLifetime;;

uniform float u_particle_minInitialSpeed;
uniform float u_particle_maxInitialSpeed;


//particle update

 

//management


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

layout(std430, binding = 2) buffer ParticleAlivePreSimIndices_t
{
    uint indices[];
}
AliveIndicesPreSim;
  

 

//attributes
layout(std430, binding = 5) buffer ParticlePosition_t
{
    vec4 positions[];
}
ParticlePositions;

layout(std430, binding = 6) buffer ParticleVelocity_t
{
    vec4 velocities[];
}
ParticleVelocities;

layout(std430, binding = 7) buffer ParticleLifetime_t
{
    float lifetimes[];
}
ParticleLifetimes;

layout(std430, binding = 8) buffer ParticleAge_t
{
    float ages[];
}
ParticleAges;



// ------------------------------------------------------------------
// FUNCTIONS --------------------------------------------------------
// ------------------------------------------------------------------

uint pop_dead_index()
{
    uint last_usable_index = atomicAdd(Counters.dead_count, -1);
    return DeadIndices.indices[last_usable_index - 1];
}

void push_alive_index(uint index)
{
    uint insert_Index = atomicAdd(Counters.alive_count[u_preSimIndex], 1);
    AliveIndicesPreSim.indices[insert_Index] = index;
}


vec3 randomPointOnSphere(float u, float v, float radius)
{
    float theta = 2 * PI * u;
    float phi = acos(2 * v - 1);
    float x = radius * sin(phi) * cos(theta);
    float y = radius * sin(phi) * sin(theta);
    float z = radius * cos(phi);
    return vec3(x, y, z);
}

float rand(vec3 co)
{
    return fract(sin(dot(co.xyz, vec3(12.9898, 78.233, 45.5432))) * 43758.5453);
}

// ------------------------------------------------------------------
// MAIN -------------------------------------------------------------
// ------------------------------------------------------------------

void main()
{
    uint index = gl_GlobalInvocationID.x;
     

    if (index <  Counters.emission_count)
    {
        uint particle_index = pop_dead_index();

        vec3 position = u_emitter_position;

       //if (u_Emission_Shape == EMISSION_SHAPE_SPHERE)
        position += randomPointOnSphere(rand(u_seeds.xyz / (index + 1)), rand(u_seeds.yzx / (index + 1)),  u_sphereRadius * rand(u_seeds.zyx / (index + 1)));
      
         
        //if (u_velocity_Mode == VELOCITY_MODE_FROM_POINT)
        vec3 direction = normalize( (position - u_emitter_position) + 0.001);  //prevent zero vector

        float initial_speed = u_particle_minInitialSpeed + 
                           (u_particle_maxInitialSpeed - u_particle_minInitialSpeed) * rand(u_seeds.xzy / (index + 1));
        float lifetime = u_particle_minLifetime + (u_particle_maxLifetime - u_particle_minLifetime) * rand(u_seeds.zyx / (index + 1));


        ParticlePositions.positions[particle_index] = vec4( position, 1.0);
        ParticleVelocities.velocities[particle_index] = vec4( direction * initial_speed, 1.0);
		ParticleLifetimes.lifetimes[particle_index] = lifetime;
        ParticleAges.ages[particle_index] = 0;

        push_alive_index(particle_index);

    }
 

}


 