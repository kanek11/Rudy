#version 460 core
// ------------------------------------------------------------------
// CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------

#define PI 3.1415926535 

#define MAX_CONSTRAINTS = 16; 

// ------------------------------------------------------------------
// INPUTS -----------------------------------------------------------
// ------------------------------------------------------------------

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------


uniform float u_deltaTime;
uniform float u_compliance;

//uniform uint u_num_distConst;

uniform uint u_num_particles;
uniform uint u_max_num_distConst;


// ------------------------------------------------------------------
// BUFFERS ---------------------------------------------------------
// ------------------------------------------------------------------


layout(std430, binding = 0) readonly buffer Positions_t
{
	vec4 positions[];

}Positions;

layout(std430, binding = 1) readonly buffer RestPositions_t
{
	vec4 position[];

}RestPositions;


layout(std430, binding = 2) buffer Corrections_t
{
	vec4 corrections[];

}Corrections;


layout(std430, binding = 3) readonly buffer InvMass_t
{
	float invMass[];

}InvMass;


layout(std430, binding = 4) readonly buffer DistConstIDs_t
{
	uint distConstIDs[];

}DistConstIDs;


//layout(std430, binding = 5) readonly buffer RestLength_t
//{
//	float restLength[];
//
//}RestLength; 
//
//
//layout(std430, binding = 6) buffer CurrentLength_t
//{
//	float currentLength[];
//
//}CurrentLength; 



void main()
{
	uint index = gl_GlobalInvocationID.x;
	if (index >= u_num_particles)
	{
		return;
	}

	 
	uint id0 = index;  
	vec4 local_correction = vec4(0.0f); 
	 

	for (uint i = 0; i < u_max_num_distConst; i++)
	{
		uint id1 =  uint( DistConstIDs.distConstIDs[index * u_max_num_distConst + i] ) ;

		if (id1 >= u_num_particles)
		{
		    break;
		}

		float invMass0 = InvMass.invMass[id0];
		float invMass1 = InvMass.invMass[id1];
		if (invMass0 == 0.0f && invMass1 == 0.0f)
		{
			continue;
		}

		vec3 restPos0 = RestPositions.position[id0].xyz;
		vec3 restPos1 = RestPositions.position[id1].xyz;
		float restLength = length(restPos0 - restPos1); 

		vec3 pos0 = Positions.positions[id0].xyz;
		vec3 pos1 = Positions.positions[id1].xyz;
		float currentLength = length(pos0 - pos1);
		if (currentLength < 0.0001f)
		{
			continue;
		} 

		//the gradient ¨ŒC
		vec3 grad = pos0 - pos1;

		float error = currentLength - restLength;
		//float lambda = error /(2 + u_compliance /u_deltaTime/u_deltaTime);
		float lambda = -error / (2);
 

		local_correction.xyz += lambda * invMass0 * (grad / currentLength);

	}  

	Corrections.corrections[id0] = local_correction;
	 

}


//if (id1 == id0 + 4)
//{
//	lambda = 100.0f;
//	//return;
//}



