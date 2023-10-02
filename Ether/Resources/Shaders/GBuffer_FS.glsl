#version 330 core  
layout(location = 0) out vec3 gPosition; 
layout(location = 1) out vec3 gAlbedo;
layout(location = 2) out vec3 gWorldNormal;
layout(location = 3) out float gSpecular;
layout(location = 4) out float gMetallic;
layout(location = 5) out float gRoughness; 


in vec3 WorldFragPos;
in vec2 TexCoords;
in vec3 WorldNormal;   //in-case  donot use bump mapping 
in vec3 WorldTangent;

 

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_SpecularMap; 
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicMap; 
uniform sampler2D u_RoughnessMap;


void main()
{
   
    gPosition = WorldFragPos;
    gWorldNormal = normalize(WorldNormal);

    gAlbedo = texture(u_AlbedoMap, TexCoords).rgb;
    //gAlbedo =  texture(u_AlbedoMap, TexCoords).rgb;
    //gAlbedo = vec4(1.0, 0.0, 0.0 ,1.0);   

    gSpecular = texture(u_SpecularMap, TexCoords).r; 
    gMetallic = texture(u_MetallicMap, TexCoords).r;  

    gRoughness = texture(u_RoughnessMap, TexCoords).r ; 

}