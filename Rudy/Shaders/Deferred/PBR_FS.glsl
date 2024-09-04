#version 460 core
//===MACRO
#define PI 3.14159265359
//=====output
layout(location = 0) out vec4 out_FragColor;

//=====input
in VS_OUT
{
    vec2 TexCoords;
}
fs_in;

uniform sampler2D gWorldPosition;
uniform sampler2D gWorldNormal;

uniform sampler2D gAlbedo;
uniform sampler2D gSpecular;
uniform sampler2D gMetallic;
uniform sampler2D gRoughness;

// IBL
uniform samplerCube u_DiffuseEnvMap;
uniform samplerCube u_SpecularEnvMap;
uniform sampler2D   u_brdfLUTTexture;

uniform bool u_EnableSkyBox;

struct DirLight
{
    vec3  color;
    vec3  direction;
    float intensity;
};

uniform DirLight u_DirLight;
uniform mat4     u_LightSpaceMatrix; // for shadow map

uniform sampler2D u_DepthTexture; // shadow map

uniform vec3 u_CameraPos;

uniform float u_min_shadow_bias;
uniform float u_max_shadow_bias;

//---shadow map
// return 1 if in shadow, 0 otherwise;
float ShadowCalculation(vec4 WorldFragPos, float NdotL)
{
    // transform to NDC;

    vec4 lightSpaceFragPos = u_LightSpaceMatrix * WorldFragPos;
    vec3 projCoords        = lightSpaceFragPos.xyz / lightSpaceFragPos.w; // perspective division
    projCoords             = projCoords * 0.5 + 0.5;

    // robust check eg: out of range of frustum , return 0 means not in shadow;
    // it's a brutal way; it might mean the frustum is too small;  we keep it in shadow to be aware of the problem;
    if (projCoords.z < 0.0 || projCoords.z > 1.0)
        return 1.0;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_DepthTexture, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    float shadow = 0.0;

    // shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    // bias       = u_shadow_bias;

    // tolerate some bias
    float bias = max(u_max_shadow_bias * (1.0 - NdotL), u_min_shadow_bias); // todo: tune it better;

    // simple 3x3 PCF for AA
    vec2 texelSize = 1.0 / textureSize(u_DepthTexture, 0);
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
        {
            float sampleDepth = texture(u_DepthTexture, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > sampleDepth ? 1.0 : 0.0;
        }
    shadow /= 9.0; // 9 samples

    return shadow;
    // return currentDepth;
    //  return closestDepth;
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom       = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // gbuffers;
    vec3 worldPos = texture(gWorldPosition, fs_in.TexCoords).rgb;

    vec3 worldNormal = texture(gWorldNormal, fs_in.TexCoords).rgb;
    if (length(worldNormal) < 0.2) discard; // discard background
    vec3 N = normalize(worldNormal * 2.0 - 1.0);

    vec3 albedo = texture(gAlbedo, fs_in.TexCoords).rgb;

    // specular or roughness workflow;
    float specularCoeff = texture(gSpecular, fs_in.TexCoords).r;

    float roughness = texture(gRoughness, fs_in.TexCoords).r; // gray
    if (roughness < 0.001) roughness = 1 - specularCoeff;     // todo: better way to be compatible with blinn-phong model;

    float metallic = texture(gMetallic, fs_in.TexCoords).r; // gray

    // Fresnel
    vec3 F0 = vec3(0.04); // default specular color
    F0      = mix(F0, albedo, metallic);

    // outgoing radiance;
    vec3 Lo = vec3(0.0);

    //======Scene direct lighting

    vec3 radiance = u_DirLight.color * u_DirLight.intensity;
    vec3 L        = -normalize(u_DirLight.direction); // minus, from shading point
    vec3 V        = normalize(u_CameraPos - worldPos);
    vec3 H        = normalize(L + V);

    // shadow  //disable for now
    float shadow = 0.5;
    // if(true)
    shadow = ShadowCalculation(vec4(worldPos, 1.0), dot(N, L));

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);

    float NdotV = dot(N, V);
    vec3  F     = vec3(0.0);
    if (NdotV >= 0.0)
        F = fresnelSchlick(NdotV, F0);

    vec3 numerator = NDF * G * F;

    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero ,also eps
    vec3  specular    = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    Lo += (1 - shadow) * (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 FragColor_HDR = Lo;
    //================================
    //=======environment;
    if (u_EnableSkyBox)
    {
        if (NdotV >= 0.0)
            F = fresnelSchlickRoughness(NdotV, F0, roughness);
        else
            F = vec3(0.0);

        kS = F;
        kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 diffuseIrradiance = texture(u_DiffuseEnvMap, N).rgb;
        vec3 diffuseEnv        = diffuseIrradiance * albedo;

        vec3        R                  = reflect(-V, N);
        const float MAX_REFLECTION_LOD = 4.0;
        // roughness = 0;
        vec3 specularColor = textureLod(u_SpecularEnvMap, R, roughness * MAX_REFLECTION_LOD).rgb;

        vec2 brdf        = texture(u_brdfLUTTexture, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specularEnv = specularColor * (F * brdf.x + brdf.y);

        FragColor_HDR = Lo + (kD * diffuseEnv + 2 * specularEnv); // todo: add ao; shadow;
    }

    // HDR tonemapping
    vec3 FragColor_LDR = FragColor_HDR / (FragColor_HDR + vec3(1.0)); // Reinhard
                                                                      // color = vec3(1.0) - exp(-color * 1.0);   //in exposure

    out_FragColor = vec4(FragColor_LDR, 1.0);
    //  out_FragColor = vec4(1.0, 0.0, 0.0, 1.0);  //debug
    // out_FragColor = vec4(vec3(shadow), 1.0); // debug for shadow

    // FragColor = vec4(V*0.5 +0.5, 1.0);
    // FragColor = vec4(vec3(dot(N, V)), 1.0);
}
