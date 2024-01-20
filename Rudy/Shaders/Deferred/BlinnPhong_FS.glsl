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
uniform sampler2D gAlbedo;
uniform sampler2D gWorldNormal;
uniform sampler2D gSpecular;
uniform sampler2D gMetallic;
uniform sampler2D gRoughness;
 

//uniform sampler2D u_SpecularMap;
//uniform sampler2D u_AlbedoMap;
//uniform sampler2D u_RoughnessMap;
//uniform sampler2D u_NormalMap;  
//uniform sampler2D u_DepthTexture;  //shadow map

  

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_LightDir;

uniform vec3 u_CameraPos;


//just trivial hack, we should let the host to handle this. but the code is not settled yet.
vec3 LightColor = vec3(1.0, 1.0, 1.0) ;
vec3 LightPos = vec3(0.0, 10.0, 10.0);
vec3 LightDir = vec3(0.0, -0.5, -1);

vec3 CameraPos = vec3(0.0, 0.0, 5.0);



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
    //light intensity
    if(u_LightColor != vec3(0.0))  
        LightColor = u_LightColor;

    if (u_LightPos != vec3(0.0))
        LightPos = u_LightPos;

    if (u_CameraPos != vec3(0.0))
        CameraPos = u_CameraPos;

    if ( u_LightDir != vec3(0.0))
         LightDir = u_LightDir;
   
    //======textures
    //normal mapping , read normal from texture 
    //vec3 normal = texture(u_NormalMap, fs_in.TexCoords).rgb;
    //normal = normal * 2.0 - 1.0;   //from [0,1] to [-1,1]
    //normal = normalize(fs_in.TBN * normal);  //from tangent space to world space
    //
    ////in case we donot use normal mapping
    //if(normal == vec3(0.0)) 
   //vec3  normal = normalize(fs_in.Normal);


    vec3 WorldPos = texture(gWorldPosition, TexCoords).rgb; 

    vec3  normal = normalize(texture(gWorldNormal, TexCoords).rgb *2.0 - 1.0); 

    vec3 baseColor = texture(gAlbedo, TexCoords).rgb;

    float specular_coeff = texture(gSpecular, TexCoords).r;   //gray for blinnphong
    if (specular_coeff == 0.0) specular_coeff = 0.4;

    float glossiness = texture(gRoughness, TexCoords).r;  //gray
    if (glossiness == 0.0) glossiness = 1.0;
    //if no glossiness set to 1;

     
    //3basic directions: l,v,h
    //vec3 lightDir = normalize(LightPos - fs_in.WorldPos);
    vec3 lightDir =  - normalize(LightDir);   //minus, from shading point
    vec3 viewDir = normalize(CameraPos - WorldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); 

    //light attenuation,simply use 1/r^2 , i found it's too much, so i use 1/r
    //float distance = length(LightPos - fs_in.WorldPos);
    //LightColor  = LightColor / (distance);
     
    // Ambient ka
    vec3 Light_ambient = vec3(0.3f);   
    vec3 ambient = Light_ambient * baseColor; 
  
    // Diffuse kd 
    float cos_d = max(dot(normal, lightDir), 0.0);

    vec3 diffuse = cos_d * baseColor * LightColor; 
    
    // Specular ks 
    float shininess = 32 * glossiness  ;  //shininess scaled;
    float cos_s = max(dot(normal, halfwayDir), 0.0);
    float highlight = pow(cos_s, shininess);
    vec3 specular = highlight * specular_coeff * LightColor; 


    //float shadow = ShadowCalculation(fs_in.LightSpaceFragPos);

    //vec3 result_color = ambient + (1-shadow)* (diffuse + specular);
    //result_color =  (1 - shadow) * (diffuse + specular);
    //result_color = ambient;
    // result_color = vec3(1-shadow);
    //for test: only show shadow value
    vec3 result_color = ambient + diffuse + specular ;

    FragColor = vec4(result_color, 1.0) ;
    // FragColor = vec4(ambient ,1.0) ;
    //FragColor = vec4(specular,1.0) ;
     //FragColor = vec4(vec3(glossiness), 1.0);
     //FragColor = vec4(baseColor, 1.0);
     

}