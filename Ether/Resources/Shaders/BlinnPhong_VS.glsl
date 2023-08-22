#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;



out VS_OUT{
     vec3 WorldPos;
     vec3 Normal;
     vec2 TexCoord;
} vs_out;
 

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    vs_out.WorldPos = vec3(u_Model * vec4(aPos, 1.0));

    vs_out.Normal = mat3(transpose(inverse(u_Model))) * aNormal;

    vs_out.TexCoord = aTexCoord;

    gl_Position = u_Projection * u_View * vec4(vs_out.WorldPos, 1.0);
}