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

uniform float u_jacobiScale;

// ------------------------------------------------------------------
// BUFFERS ---------------------------------------------------------
// ------------------------------------------------------------------


layout(std430, binding = 0) buffer Positions_t
{
	vec4 positions[];

}Positions;

layout(std430, binding = 1) readonly buffer PrevPositions_t   
{ 
	vec4 prev_positions[]; 

}PrevPositions;

layout(std430, binding = 2)  buffer Corrections_t   
{ 
	vec4 corrections[]; 

}Corrections; 
 
 layout(std430, binding = 3) buffer Velocities_t
{
	vec4 velocities[];

}Velocities;  

 layout(std430, binding = 4) readonly buffer InvMass_t
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

	float invMass = InvMass.invMass[index] ; 
	if (invMass == 0.0f)
	{
		return;
	}
	
	//add the correction to the position
	Positions.positions[index].xyz += Corrections.corrections[index].xyz * 0.2;// *u_jacobiScale;
	
	//compute the velocity
	Velocities.velocities[index].xyz = (Positions.positions[index].xyz - PrevPositions.prev_positions[index].xyz) / u_deltaTime;

	Corrections.corrections[index] = vec4(0.0f);
	
}
