#version 330 core  
layout(location = 0) out vec3 gPosition; 
layout(location = 1) out vec3 gAlbedo;
layout(location = 2) out vec3 gWorldNormal;
layout(location = 3) out vec3 gWorldTangent;
layout(location = 4) out float gSpecular;
layout(location = 5) out float gMetallic;
layout(location = 6) out float gRoughness; 
 

in vec3 WorldPos;
in vec2 TexCoords;
in vec3 WorldNormal;   //in-case  donot use bump mapping 
in vec3 WorldTangent;

in mat3 WorldTBN; 


in float test_flag;
 

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_SpecularMap; 
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicMap; 
uniform sampler2D u_RoughnessMap;


void main()
{
   
    gPosition = WorldPos;

    //if(test_flag!=0)
    gAlbedo = texture(u_AlbedoMap, TexCoords).rgb;
    //else
    //gAlbedo = vec3(0.0, 1.0, 0.0);   

    gSpecular = texture(u_SpecularMap, TexCoords).r;
    gMetallic = texture(u_MetallicMap, TexCoords).r; 
    gRoughness = texture(u_RoughnessMap, TexCoords).r;


    //gWorldNormal = normalize(WorldNormal); 

    gWorldTangent = normalize(WorldTangent) * 0.5 + 0.5; //from [-1,1] to [0,1]

    vec3 localNormal = texture(u_NormalMap, TexCoords).rgb * 2.0 - 1.0; //from [0,1] to [-1,1] 
    gWorldNormal = normalize( WorldTBN * localNormal);   //convert from tangent space to world space

    //gWorldNormal = vec3( 1.0,0.0,0.0);  //test normal map
    gWorldNormal = gWorldNormal * 0.5 + 0.5; //from [-1,1] to [0,1] ;


  

}