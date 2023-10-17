#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

const float PI = 3.14159265359;


 
uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gWorldNormal;
uniform sampler2D gSpecular;
uniform sampler2D gMetallic;
uniform sampler2D gRoughness;


//IBL 
uniform samplerCube u_DiffuseEnvMap;
uniform samplerCube u_SpecularEnvMap;
uniform sampler2D u_BrdfLUT;


//uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_LightDir;

uniform vec3 u_CameraPos;


vec3 LightColor = vec3(1.0, 1.0, 1.0);
//vec3 LightPos = vec3(0.0, 10.0, 10.0);
vec3 LightDir = vec3(0.0, -0.5, -1);

//vec3 CameraPos = vec3(0.0, 0.0, 5.0);


// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// -----

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}








void main()
{
    //light intensity
    if (u_LightColor != vec3(0.0))
        LightColor = u_LightColor; 

    if (u_LightDir != vec3(0.0))
        LightDir = u_LightDir;


    //gbuffers;
    vec3 worldFragPos = texture(gPosition, TexCoords).rgb; 

    vec3 N = normalize(texture(gWorldNormal, TexCoords).rgb * 2.0 - 1.0);

    vec3 albedo = texture(gAlbedo, TexCoords).rgb;  

    float roughness = texture(gRoughness, TexCoords).r;  //gray
    float metallic = texture(gMetallic, TexCoords).r;  //gray  
   

    //Fresnel
    vec3 F0 = vec3(0.04);  //default specular color 
    F0 = mix(F0, albedo, metallic);

    //outgoing radiance;
    vec3 Lo = vec3(0.0);


    //======Scene lighting
    vec3 radiance = LightColor; //for now;

     


    vec3 L = -normalize(LightDir);   //minus, from shading point
    vec3 V = normalize(u_CameraPos - worldFragPos);
    vec3 H = normalize(L + V);
     

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);

    float NdotV = dot(N, V);
    vec3 F = vec3(0.0);
    if (NdotV >= 0.0)
      F = fresnelSchlick(NdotV, F0); 


    vec3 numerator = NDF * G * F;

    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero ,also eps
    vec3 specular = numerator / denominator;


    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;


    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;



    //=======environment; 

 
    if (NdotV >= 0.0)  
        F = fresnelSchlickRoughness(NdotV, F0, roughness);
    else
        F = vec3(0.0);


    kS = F;
    kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 diffuseIrradiance = texture(u_DiffuseEnvMap, N).rgb;
    vec3 diffuseEnv = diffuseIrradiance * albedo;
     

    vec3 R = reflect(-V, N); 
    const float MAX_REFLECTION_LOD = 4.0;

    //roughness = 0;
    vec3 specularColor = 3 * textureLod(u_SpecularEnvMap, R, roughness * MAX_REFLECTION_LOD).rgb;
 

    vec2 brdf = texture(u_BrdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specularEnv = specularColor * (F * brdf.x + brdf.y);

     
    vec3 FragColor_HDR = Lo +  ( kD * diffuseEnv + specularEnv);  //todo: add ao;
     
    // HDR tonemapping
    vec3 FragColor_LDR = FragColor_HDR / (FragColor_HDR + vec3(1.0));  //Reinhard
    // color = vec3(1.0) - exp(-color * 1.0);   //in exposure
     

    FragColor = vec4(FragColor_LDR, 1.0);
     


    //FragColor = vec4(V*0.5 +0.5, 1.0);
    //FragColor = vec4(vec3(dot(N, V)), 1.0);



}











