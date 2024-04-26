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

// engine provided
uniform float u_deltaTime;

// system

// emitter

uniform uint u_preSimIndex;
uniform uint u_postSimIndex;

// simulation parameters

// uniform sampler2D s_Depth;
// uniform sampler2D s_Normals;

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

layout(std430, binding = 3) buffer ParticleAlivePostSimIndices_t
{
    uint indices[];
}
AliveIndicesPostSim;

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

// add a simple collision detection with a xz plane
// if the particle is below the plane, bounce it back up

void collision_detection(vec4 position, vec4 velocity)
{
}

void push_dead_index(uint index)
{
    uint insert_Index                 = atomicAdd(Counters.dead_count, 1);
    DeadIndices.indices[insert_Index] = index;
}

uint pop_dead_index()
{
    uint index = atomicAdd(Counters.dead_count, -1);
    return DeadIndices.indices[index - 1];
}

void push_alive_index(uint index)
{
    uint insert_Index                         = atomicAdd(Counters.alive_count[u_postSimIndex], 1);
    AliveIndicesPostSim.indices[insert_Index] = index;
}

uint pop_alive_index()
{
    uint index = atomicAdd(Counters.alive_count[u_preSimIndex], -1);
    return AliveIndicesPreSim.indices[index - 1];
}

// ------------------------------------------------------------------
// MAIN -------------------------------------------------------------
// ------------------------------------------------------------------

void main()
{
    uint index = gl_GlobalInvocationID.x;

    if (index < Counters.update_count)
    {
        // Consume an Alive particle index
        uint particle_index = pop_alive_index();

        // Particle particle = ParticleData.particles[particle_index];
        vec4  position = ParticlePositions.positions[particle_index];
        vec4  velocity = ParticleVelocities.velocities[particle_index];
        float lifetime = ParticleLifetimes.lifetimes[particle_index];
        float age      = ParticleAges.ages[particle_index];

        // Is it dead?
        if (age >= lifetime)
        {
            // If dead, just append into the DeadIndices list
            push_dead_index(particle_index);
        }
        else
        {
            // If still alive, increment lifetime and run simulation
            age += u_deltaTime;

            // hardcode graivity for now
            velocity.y += -9.8 * u_deltaTime;
            // hardcode drag for now
            velocity *= 0.99;
            position += velocity * u_deltaTime;

            // very simple collision detection
            if (position.y < +0.5)
            {
                position.y = +0.5;
                velocity.y = -velocity.y * 0.8;
            }

            // Update particle data
            ParticlePositions.positions[particle_index]   = position;
            ParticleVelocities.velocities[particle_index] = velocity;
            ParticleAges.ages[particle_index]             = age;

            // Append index back into AliveIndices list
            push_alive_index(particle_index);

            // Increment draw instance count
            // atomicAdd(ParticleRenderArgs.instance_count, 1);
        }
    }
}
