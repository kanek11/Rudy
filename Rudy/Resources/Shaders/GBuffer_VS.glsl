#version 330 core  
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent; 
layout(location = 3) in vec2 aTexCoords;
 //bitangent to be compute by cross product; this saves a buffer slot
 
  
 
out vec3 WorldPos;
out vec2 TexCoords; 
out vec3 WorldNormal;   //in-case  donot use bump mapping 
out vec3 WorldTangent; 

out mat3 WorldTBN;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;


void main()
{
    WorldPos = vec3(u_Model * vec4(aPos, 1.0));

    TexCoords = aTexCoords;

    WorldNormal = mat3(transpose(inverse(u_Model))) * aNormal;
    WorldTangent = mat3(transpose(inverse(u_Model))) * aTangent; 

    vec3 WorldBitangent = cross(WorldNormal, WorldTangent);
    WorldTBN = mat3(WorldTangent, WorldBitangent, WorldNormal);

    gl_Position = u_ProjectionView * vec4(WorldPos, 1.0);

}