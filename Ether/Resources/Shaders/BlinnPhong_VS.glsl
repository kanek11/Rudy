#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;


out VS_OUT{
     vec3 WorldFragPos; 
     vec2 TexCoords;
     mat3 TBN;
     vec3 Normal; //in-case  donot use bump mapping
     vec4 LightSpaceFragPos; //
} vs_out;
 

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform mat4 u_LightViewMat;

void main()
{
    vs_out.WorldFragPos = vec3(u_Model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;

    vs_out.Normal = mat3(transpose(inverse(u_Model))) * aNormal;

    vec3 T = normalize(vec3(u_Model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(u_Model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(u_Model * vec4(aNormal, 0.0)));

    vs_out.TBN = mat3(T, B, N); 

    vs_out.LightSpaceFragPos= u_LightViewMat * vec4(vs_out.WorldFragPos, 1.0);

    gl_Position = u_Projection * u_View * vec4(vs_out.WorldFragPos, 1.0);
}