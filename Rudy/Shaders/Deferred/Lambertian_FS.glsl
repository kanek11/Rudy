#version 460 core
//===MACRO
#define PI 3.14159265359

//=====output
layout(location = 0) out vec4 out_FragColor;

//=====input
in VS_OUT
{
    vec2 TexCoords;

} fs_in;


uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gWorldNormal;
uniform sampler2D gSpecular;
uniform sampler2D gMetallic;
uniform sampler2D gRoughness;


uniform sampler2D u_DepthTexture;  //shadow map
 


struct DirLight {
    vec3 color;
    vec3 direction;
    float intensity;
};
 
uniform DirLight u_DirLight;
uniform mat4 u_LightSpaceMatrix;  //for shadow map

uniform vec3 u_CameraPos;


void main()
{


    //gbuffers;
    vec3 worldPos = texture(gPosition, fs_in.TexCoords).rgb;
    vec3 worldNormal = texture(gWorldNormal, fs_in.TexCoords).rgb;
    if (length(worldNormal) < 0.1) discard;  //discard background
    vec3 N = normalize(worldNormal * 2.0 - 1.0);
  
    
    vec3 albedo = texture(gAlbedo, fs_in.TexCoords).rgb;
    albedo = pow(albedo, vec3(2.2));  //gamma correction
    albedo = vec3(1.0, 1.0, 1.0); //debug: white

    float roughness = texture(gRoughness, fs_in.TexCoords).r;  //gray
  

    //outgoing radiance;
    vec3 Lo = vec3(0.0);


    //======Scene lighting

    vec3 radiance = u_DirLight.color * u_DirLight.intensity;
    vec3 L = - normalize(u_DirLight.direction);   //minus, from shading point
 
    float LdotN = max(dot(L, N), 0.0);  
     
    Lo += 2 * (albedo / PI) * radiance * LdotN; 

    out_FragColor = vec4(Lo, 1.0);


}