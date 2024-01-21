#version 460 core  
layout(location = 0) out vec4 gWorldPosition; 
layout(location = 1) out vec4 gAlbedo;
layout(location = 2) out vec4 gWorldNormal;
layout(location = 3) out vec4 gWorldTangent;
layout(location = 4) out float gSpecular;
layout(location = 5) out float gMetallic;
layout(location = 6) out float gRoughness; 
 

in VS_OUT
{
    vec3 WorldPos;
    vec2 TexCoords;
    vec3 WorldNormal;
    vec3 WorldTangent;
    mat3 WorldTBN;

} fs_in;

  
uniform sampler2D u_AlbedoMap; 
uniform sampler2D u_SpecularMap;  
uniform sampler2D u_NormalMap; 
uniform sampler2D u_MetallicMap;  
uniform sampler2D u_RoughnessMap;
 
uniform vec3 u_Albedo;  
uniform float u_Specular;
uniform float u_Metallic;
uniform float u_Roughness; 

uniform bool Use_u_AlbedoMap;
uniform bool Use_u_SpecularMap;
uniform bool Use_u_NormalMap;
uniform bool Use_u_MetallicMap;
uniform bool Use_u_RoughnessMap;


//there's no point branching in shader.
//not if we can write a shader generation kinda thing, this can't be optimized.

//also once varying, 
//the uniform has to set every frame,  hard-coded this way.

void main()
{
   
    gWorldPosition.xyz = fs_in.WorldPos;

    gAlbedo.rgb = u_Albedo;
    if (Use_u_AlbedoMap)
    {
        gAlbedo *= texture(u_AlbedoMap, fs_in.TexCoords).rgba;
        //gAlbedo = pow( texture(u_AlbedoMap, fs_in.TexCoords).rgb, vec3(2.2) ); //gamma correction
 
    }
	     

    gSpecular.r = u_Specular;
   if(Use_u_SpecularMap)
	 gSpecular *= texture(u_SpecularMap, fs_in.TexCoords).r;

	gMetallic.r = u_Metallic;
    if(Use_u_MetallicMap)
		gMetallic *= texture(u_MetallicMap, fs_in.TexCoords).r;

	gRoughness.r = u_Roughness;
    if(Use_u_RoughnessMap)
		gRoughness *= texture(u_RoughnessMap, fs_in.TexCoords).r;

    gWorldNormal.rgb = normalize(fs_in.WorldNormal); 
    if (Use_u_NormalMap)
    {
        vec3 localNormal = texture(u_NormalMap, fs_in.TexCoords).rgb * 2.0 - 1.0; //from [0,1] to [-1,1] 
        gWorldNormal.rgb = normalize(fs_in.WorldTBN * localNormal) ;   //convert from tangent space to world space
    }
  

    //gfs_in.WorldNormal = vec3( 1.0,0.0,0.0);  //test normal map
    gWorldNormal.rgb = gWorldNormal.rgb * 0.5 + 0.5; //from [-1,1] to [0,1] ; 
    gWorldTangent.rgb = normalize(fs_in.WorldTangent) * 0.5 + 0.5; //from [-1,1] to [0,1] 
     

}