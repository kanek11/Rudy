#version 460 core
// ------------------------------------------------------------------
// CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------

#define PI 3.1415926535

// ------------------------------------------------------------------
// INPUTS -----------------------------------------------------------
// ------------------------------------------------------------------

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

uniform float u_deltaTime; 
uniform uint u_num_particles;

// ------------------------------------------------------------------
// BUFFERS ---------------------------------------------------------
// ------------------------------------------------------------------


layout(std430, binding = 0) buffer Positions_t
{
	vec4 positions[];

}Positions;

layout(std430, binding = 1) buffer PrevPositions_t
{
	vec4 prev_positions[];

}PrevPositions;
 

layout(std430, binding = 3) buffer Velocities_t
{
	vec4 velocities[];

}Velocities;

layout(std430, binding = 4) buffer InvMass_t
{
	float invMass[];

}InvMass;




void main()
{
	uint index = gl_GlobalInvocationID.x;
	if (index >= u_num_particles)
	{
		return;
	}

	float invMass = InvMass.invMass[index]; 
	if (invMass == 0.0f)
	{
		return;
	} 

	
}
