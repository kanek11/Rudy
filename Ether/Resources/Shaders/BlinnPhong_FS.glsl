#version 330 core

out vec4 FragColor;

in VS_OUT{
     vec3 WorldFragPos;
     vec2 TexCoords;
     mat3 TBN;
     vec3 Normal; //in-case  donot use bump mapping
     vec4 LightSpaceFragPos; //
} fs_in;
 
 

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;
uniform sampler2D u_NormalMap; 

uniform sampler2D u_DepthMap;  //shadow map

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;

uniform vec3 u_CameraPos;


//just trivial hack, we should let the host to handle this. but the code is not settled yet.
vec3 LightColor = vec3(1.0, 1.0, 1.0) * 1000;
vec3 LightPos = vec3(0.0, 10.0, 10.0);
vec3 CameraPos = vec3(0.0, 0.0, 5.0);



//formula:
//ambient = ka * Ia  ,  ka is basically basecolor,  Ia is light intensity of environment/ambient light
//diffuse = kd * I   * cos(theta) ,   
//specular = ks * I  * cos(alpha)^shininess


//return 1 if in shadow, 0 otherwise;
float ShadowCalculation(vec4 lightSpaceFragPos)
{ 
    // transform to [0,1] range

    vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;   //perspective division
      projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_DepthMap, projCoords.xy).r;

    //return 0 if no depth value could be sampled
    // if (closestDepth == 0.0)  return 0;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}




void main()
{ 
    //light intensity
    if(u_LightColor != vec3(0.0))  
        LightColor = u_LightColor;

    if (u_LightPos != vec3(0.0))
        LightPos = u_LightPos;

    if (u_CameraPos != vec3(0.0))
        CameraPos = u_CameraPos;

    //======textures
    //normal mapping , read normal from texture 
    vec3 normal = texture(u_NormalMap, fs_in.TexCoords).rgb;
    normal = normal * 2.0 - 1.0;   //from [0,1] to [-1,1]
    normal = normalize(fs_in.TBN * normal);  //from tangent space to world space

    //in case we donot use normal mapping
    if(normal == vec3(0.0))  normal = normalize(fs_in.Normal);

     
    float gamma = 2.2;
    vec3 BaseColor = pow(texture(u_DiffuseMap, fs_in.TexCoords).rgb, vec3(gamma));  //element wise function;

    vec3 SpecularColor = texture(u_SpecularMap, fs_in.TexCoords).rgb;  //non-color data. 

   
    //3basic directions: l,v,h
    vec3 lightDir = normalize(LightPos - fs_in.WorldFragPos);
    vec3 viewDir = normalize(CameraPos - fs_in.WorldFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); 

    //light attenuation,simply use 1/r^2
    float distance = length(LightPos - fs_in.WorldFragPos);
    LightColor  = LightColor / (distance * distance);
     
    // Ambient ka
    vec3 Light_ambient = vec3(1);   
    vec3 ambient = Light_ambient * BaseColor; 
  
    // Diffuse kd 
    float cos_d = max(dot(normal, lightDir), 0.0);

    vec3 diffuse = cos_d * BaseColor * LightColor;

    
    // Specular ks 
    int shininess = 32;
    float cos_s = pow(max(dot(normal, halfwayDir), 0.0), shininess); 
    vec3 specular = cos_s * SpecularColor * LightColor; 


    float shadow = ShadowCalculation(fs_in.LightSpaceFragPos);

     vec3 result_color = ambient + (1-shadow)* (diffuse + specular);
    //vec3 result_color = vec3(1-shadow);
    //for test: only show shadow value

    FragColor = vec4(result_color, 1.0) ;
    //FragColor = vec4(1.0,0.0,0.0, 1.0);
}