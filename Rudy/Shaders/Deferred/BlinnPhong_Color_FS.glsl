#version 460 core

out vec4 FragColor;

//in VS_OUT{
//     vec3 WorldPos;
//     vec2 TexCoords;
//     mat3 TBN;
//     vec3 Normal; //in-case  donot use bump mapping
//     vec4 LightSpaceFragPos; //
//} fs_in;


in vec2 TexCoords;

uniform sampler2D gWorldPosition;
//uniform sampler2D gAlbedo;
uniform sampler2D gWorldNormal;
uniform sampler2D gSpecular;
uniform sampler2D gMetallic;
uniform sampler2D gRoughness;
  

uniform vec3 u_Albedo;
uniform vec3 u_CameraPos;

struct DirLight {
    vec3 color;
    vec3 direction;
    float intensity;
};
 
uniform DirLight u_DirLight;


//formula:
//ambient = ka * Ia  ,  ka is basically baseColor,  Ia is light intensity of environment/ambient light
//diffuse = kd * I   * cos(theta) ,   
//specular = ks * I  * cos(alpha)^shininess


//
//    //return 0 if no depth value could be sampled
//    // if (closestDepth == 0.0)  return 0;

//return 1 if in shadow, 0 otherwise;
//float ShadowCalculation(vec4 lightSpaceFragPos)
//{ 
//    // transform to [0,1] range
//
//    vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;   //perspective division
//      projCoords = projCoords * 0.5 + 0.5;
//    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//    float closestDepth = texture(u_DepthTexture, projCoords.xy).r;

//
//    // get depth of current fragment from light's perspective
//    float currentDepth = projCoords.z;
//
//    //tolerate some bias 
//    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 
//    // check whether current frag pos is in shadow
//
//    float bias = 0.00005;
//    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
//
//
//    return shadow;
//}




void main()
{ 


    vec3 WorldPos = texture(gWorldPosition, TexCoords).rgb; 

    vec3 normal = normalize(texture(gWorldNormal, TexCoords).rgb * 2.0 - 1.0);

    vec3 baseColor = u_Albedo;  //texture(gAlbedo, TexCoords).rgb;

    float specular_coeff = texture(gSpecular, TexCoords).r;   //gray for blinnphong
    if (specular_coeff == 0.0) specular_coeff = 0.4;

    float glossiness = texture(gRoughness, TexCoords).r;  //gray
    if (glossiness == 0.0) glossiness = 1.0;
    //if no glossiness set to 1;


    vec3 lightColor = u_DirLight.color * u_DirLight.intensity;

    //3basic directions: l,v,h
    //vec3 lightDir = normalize(LightPos - fs_in.WorldPos);
    vec3 lightDir = -normalize(u_DirLight.direction);   //minus, from shading point
    vec3 viewDir = normalize(CameraPos - WorldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);


    // Ambient ka
    vec3 ambient_light = vec3(0.3f);
    vec3 ambient = ambient_light * baseColor;

    // Diffuse kd 
    float cos_d = max(dot(normal, lightDir), 0.0);

    vec3 diffuse = cos_d * baseColor * lightColor;

    // Specular ks 
    float shininess = 32 * glossiness;  //shininess scaled;
    float cos_s = max(dot(normal, halfwayDir), 0.0);
    float highlight = pow(cos_s, shininess);
    vec3 specular = highlight * specular_coeff * lightColor;


    //float shadow = ShadowCalculation(fs_in.LightSpaceFragPos);

    //vec3 result_color = ambient + (1-shadow)* (diffuse + specular);
    //result_color =  (1 - shadow) * (diffuse + specular);
    //result_color = ambient;
    // result_color = vec3(1-shadow);
    //for test: only show shadow value


    vec3 result_color = ambient + diffuse + specular;

    FragColor = vec4(result_color, 1.0); 


}