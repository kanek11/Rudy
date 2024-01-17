#version 460 core
#extension GL_NV_shader_atomic_float : require


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
uniform float u_compliance;

uniform uint u_num_distConst;

// ------------------------------------------------------------------
// BUFFERS ---------------------------------------------------------
// ------------------------------------------------------------------


layout(std430, binding = 0) readonly buffer Positions_t
{
	vec4 positions[];

}Positions;


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


layout(std430, binding = 5) readonly buffer RestLength_t
{
	float restLength[];

}RestLength;




void main()
{
	uint index = gl_GlobalInvocationID.x;
	if (index >= u_num_distConst)
	{
		return;
	}

	//only difference for now with stretch;
	uint id0 = DistConstIDs.distConstIDs[index * 4 + 2];
	uint id1 = DistConstIDs.distConstIDs[index * 4 + 3];

	float invMass0 = InvMass.invMass[id0];
	float invMass1 = InvMass.invMass[id1];
	if (invMass0 == 0.0f && invMass1 == 0.0f)
	{
		return;
	}

	float restLength = RestLength.restLength[index];

	vec3 pos0 = Positions.positions[id0].xyz;
	vec3 pos1 = Positions.positions[id1].xyz;

	//the gradient ¨ŒC
	vec3 grad = pos0 - pos1;
	float currentLength = length(grad);
	if (currentLength < 0.0001f)  //degenerate to a point somehow, should be avoided
	{
		return;
	}

	float error = currentLength - restLength;
	float lambda = - error / (invMass0 + invMass1 + u_compliance / u_deltaTime / u_deltaTime);

	vec3 correction0 = lambda * invMass0 * (grad / currentLength);
	vec3 correction1 = -lambda * invMass1 * (grad / currentLength);


	atomicAdd(Corrections.corrections[id0].x, correction0.x);
	atomicAdd(Corrections.corrections[id0].y, correction0.y);
	atomicAdd(Corrections.corrections[id0].z, correction0.z);

	atomicAdd(Corrections.corrections[id1].x, correction1.x);
	atomicAdd(Corrections.corrections[id1].y, correction1.y);
	atomicAdd(Corrections.corrections[id1].z, correction1.z);


}



//Corrections.corrections[id0].xyz += lambda * invMass0 * (grad / currentLength);
//Corrections.corrections[id1].xyz -= lambda * invMass1 * (grad / currentLength);