#version 330 core  
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;  //to be compute by cross product; this saves a buffer slot
 
 
out vec3 WorldFragPos;
out vec2 TexCoords; 
out vec3 WorldNormal;   //in-case  donot use bump mapping 
out vec3 WorldTangent; 


uniform mat4 u_Model;
uniform mat4 u_ProjectionView;


void main()
{
    WorldFragPos = vec3(u_Model * vec4(aPos, 1.0));

    TexCoords = aTexCoords;

    WorldNormal = mat3(transpose(inverse(u_Model))) * aNormal;
    WorldTangent = mat3(transpose(inverse(u_Model))) * aTangent; 

    gl_Position = u_ProjectionView * vec4(WorldFragPos, 1.0);

}