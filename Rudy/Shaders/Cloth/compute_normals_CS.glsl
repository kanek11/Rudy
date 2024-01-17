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

uniform uint u_num_triangles;


// ------------------------------------------------------------------
// BUFFERS ---------------------------------------------------------
// ------------------------------------------------------------------

layout(std430, binding = 0) readonly buffer Positions_t
{
	vec4 positions[];

}Positions;


layout(std430, binding = 1) buffer Normals_t
{
    vec4 normals[];

}Normals;


layout(std430, binding = 2) buffer Indices_t
{
	uint indices[];

}Indices;


 


void main()
{
	uint index = gl_GlobalInvocationID.x;
    if ( index >= u_num_triangles) return;
	
	//query vertex id
	uint id0 = Indices.indices[index * 3 + 0];
	uint id1 = Indices.indices[index * 3 + 1];
    uint id2 = Indices.indices[index * 3 + 2];
    
    //surface normal
    vec3 pos0 = Positions.positions[id0].xyz;
    vec3 pos1 = Positions.positions[id1].xyz;
    vec3 pos2 = Positions.positions[id2].xyz;
    
    vec3 normal = normalize(cross(pos1 - pos0, pos2 - pos0));
    
    atomicAdd(Normals.normals[id0].x, normal.x);
    atomicAdd(Normals.normals[id0].y, normal.y);
    atomicAdd(Normals.normals[id0].z, normal.z); 

    atomicAdd(Normals.normals[id1].x, normal.x);
    atomicAdd(Normals.normals[id1].y, normal.y);
    atomicAdd(Normals.normals[id1].z, normal.z);
    
    atomicAdd(Normals.normals[id2].x, normal.x);
    atomicAdd(Normals.normals[id2].y, normal.y);
    atomicAdd(Normals.normals[id2].z, normal.z);


	 
}
