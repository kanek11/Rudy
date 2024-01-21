#version 460 core
const float PI = 3.14159265359;

layout(local_size_x = 1024) in;

//system.
uniform float u_deltaTime;

uniform vec3  u_acceleration;
 
uniform vec3 u_random;

//emitter 
uniform vec3  u_emitter_world_pos; 
 
//particle spawn 
uniform vec2  u_start_position_min_max;
uniform vec2  u_start_velocity_min_max;

//particles 
uniform float u_particle_lifetime;
uniform float u_particle_age;    


 
layout(std430, binding = 0) buffer Positions
{
    vec4 positions[];
};

layout(std430, binding = 1) buffer Velocities
{
    vec4 velocities[];
};

layout(std430, binding = 2) buffer Ages
{
    float ages[];
};

layout(std430, binding = 3) buffer Rotations
{
    vec2 rotations[];
};

vec3 random_initial_velocity()
{

    float velocity = mix(u_start_velocity_min_max.x, u_start_velocity_min_max.y, u_random.z);

   //float theta = mix(0.0, u_cone_angle, u_random.x);
   //float phi = mix(0.0, 2.0 * PI, u_random.y);
   //vec3  v = vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
   //
   //return normalize(u_emitter_basis * v * u_direction_constraints) * velocity;

    return vec3(velocity);
}

vec3 random_initial_position()
{
    float offset = mix(u_start_position_min_max.x, u_start_position_min_max.y, u_random.y);

    return u_emitter_world_pos + vec3(offset, 0, 0);
}

//float randomInitialRotationalVelocity()
//{
//    return mix(-u_start_rotational_velocity_min_max.x, u_start_rotational_velocity_min_max.y, u_random.x);
//}

void main()
{ 
    uint idx = gl_GlobalInvocationID.x;

    vec3  in_position = positions[idx].xyz;
    vec3  in_velocity = velocities[idx].xyz;
    float in_age = ages[idx];
    vec2  in_rotation = rotations[idx];

    if (in_age > u_particle_lifetime)
    {
        // Particle is dead - recycle
        positions[idx] = vec4(random_initial_position(), 1.0);
        velocities[idx] = vec4(random_initial_velocity(), 0.0);
        //rotations[idx] = vec2(0.0, randomInitialRotationalVelocity()); 
        ages[idx] = 0.0;
      
    }
    else
    {
        // Particle is alive - animate
        velocities[idx] = vec4(in_velocity + u_acceleration * u_deltaTime, 0.0);
        positions[idx] = vec4(in_position + in_velocity * u_deltaTime + 0.5 * u_acceleration * u_deltaTime * u_deltaTime, 1.0);
     
        ages[idx] = in_age + u_deltaTime;
        //rotations[idx].x = mod(in_rotation.x + in_rotation.y * u_deltaTime, 2.0 * PI);
    }
}