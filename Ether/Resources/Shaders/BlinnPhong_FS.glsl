#version 330 core

out vec4 FragColor;

in VS_OUT{
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;
 

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;

uniform vec3 u_ViewPos;


//just trivial hack, we should let the host to handle this. but the code is not settled yet.
vec3 LightColor = vec3(1.0, 1.0, 1.0);
vec3 LightPos = vec3(0.0, 5.0, 5.0);
vec3 ViewPos = vec3(0.0, 0.0, 5.0);

void main()
{

    //we just implement the very basic of blinn-phong here.


    //light intensity
    if(u_LightColor != vec3(0.0))  
        LightColor = u_LightColor;

    if (u_LightPos != vec3(0.0))
        LightPos = u_LightPos;

    if (u_ViewPos != vec3(0.0))
        ViewPos = u_ViewPos;



    float gamma = 2.2;
    vec3 BaseColor = pow(texture(u_DiffuseMap, fs_in.TexCoord).rgb, vec3(gamma));  //element wise function;

    // Ambient ka
    vec3 k_ambient = vec3(0.2);
    vec3 ambient = k_ambient * BaseColor;
    //vec3 ambient = k_ambient * 5;

    vec3 norm = normalize(fs_in.Normal);
    //l,v,h
    vec3 lightDir = normalize(LightPos - fs_in.WorldPos);
    vec3 viewDir = normalize(ViewPos - fs_in.WorldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); 
  
    // Diffuse kd 
    float cos_d = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = cos_d * BaseColor;

    
    // Specular ks 
    int shininess = 32;
    float cos_s = pow(max(dot(norm, halfwayDir), 0.0), shininess);

    vec3 specular = cos_s * texture(u_SpecularMap, fs_in.TexCoord).rgb;

    vec3 result_color = (ambient + diffuse + specular);

    FragColor = vec4(result_color, 1.0) ;
    //FragColor = vec4(1.0,0.0,0.0, 1.0);
}