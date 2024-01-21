#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
//bitangent to be compute by cross product; this saves a buffer slot



out VS_OUT
{
    vec3 WorldPos;
    vec2 TexCoords;
    vec3 WorldNormal;
    vec3 WorldTangent;
    mat3 WorldTBN;

} vs_out;



uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;


void main()
{

    vs_out.TexCoords = aTexCoords;
    vs_out.WorldNormal = mat3(transpose(inverse(u_model))) * aNormal;
    vs_out.WorldTangent = mat3(transpose(inverse(u_model))) * aTangent;


    vs_out.WorldPos = vec3(u_model * vec4(aPos, 1.0));

    //warning: make sure the handedness is correct
    vec3 WorldBitangent = cross(vs_out.WorldNormal, vs_out.WorldTangent);
    vs_out.WorldTBN = mat3(vs_out.WorldTangent, WorldBitangent, vs_out.WorldNormal);

    gl_Position = u_projection * u_view * vec4(vs_out.WorldPos, 1.0);

}