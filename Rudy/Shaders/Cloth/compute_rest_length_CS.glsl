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

// ------------------------------------------------------------------
// BUFFERS ---------------------------------------------------------
// ------------------------------------------------------------------

layout(std430, binding = 0) buffer RestPositions_t
{
    vec4 position[];
}
RestPositions;

layout(std430, binding = 1) buffer DistConstIDs_t
{
    uint distConstIds[];
}
DistConstIDs;

layout(std430, binding = 2) buffer RestLength_t
{
    float restLength[];
}
RestLength;

void main()
{
    uint index = gl_GlobalInvocationID.x;

    vec4 p1 = RestPositions.position[DistConstIDs.distConstIds[index * 2]];
    vec4 p2 = RestPositions.position[DistConstIDs.distConstIds[index * 2 + 1]];

    RestLength.restLength[index] = length(p1 - p2);
}
