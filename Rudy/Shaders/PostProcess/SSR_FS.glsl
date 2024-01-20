#version 460 core

out vec4 FragColor;

in vec2 TexCoords;


uniform sampler2D gWorldPosition;
uniform sampler2D gWorldNormal;
uniform sampler2D gScreenDepth; 

uniform sampler2D u_LightingPassTexture;

uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 u_CameraPos;
 

// required when using a perspective projection matrix
//float LinearizeDepth(float depth)
//{
//    float zValue = depth * 2.0 - 1.0; // Back to NDC  
//    return (2.0 * near * far) / (far + near - zValue * (far - near));
//}

 


void main()
{ 
    vec3 WorldPos = texture(gWorldPosition, TexCoords).rgb;
    //vec3 screenColor = texture(u_LightingPassTexture, TexCoords).rgb;
    vec3 normal = texture(gWorldNormal, TexCoords).rgb * 2.0 - 1.0;
    float screenDepth = texture(gScreenDepth, TexCoords).r;
     
     
    vec3 viewDir = normalize(u_CameraPos - WorldPos); 
    vec3 indirColor = vec3(0.0);
    vec2 sampleUV = vec2(0.0); 
     vec3 currentPos = WorldPos;

    vec3 reflectedRay = vec3(0.0); // Compute the reflected ray
    //vec3 reflectedRay = 2.0 * dot(viewDir, normal) * normal - viewDir; // Compute the reflected ray

    int hit = 0;
    //not background, and not backface
    if (screenDepth < 1 && dot(viewDir, normal) > 0.0 )
    {  
        reflectedRay = normalize(reflect(-viewDir, normal));
        currentPos += reflectedRay * 0.01; //avoid self-intersections

        // Raymarching loop for reflection  
        for (int i = 0; i < 512; ++i) // march 32 steps for example
        {
            currentPos += reflectedRay * 0.01; // Step along the reflected ray
 
            vec3 currentViewPos = (u_view * vec4(currentPos, 1.0)).xyz;
            float currentDepth = currentViewPos.z;
 
            vec4 clipCoords = u_projection * u_view * vec4(currentPos, 1.0);
            vec3 ndc = clipCoords.xyz / clipCoords.w;
            sampleUV = ndc.xy * 0.5 + 0.5;  
            //out of screen space
            if (sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0)
            {
				break;
			}

            float currentFragDepth = texture(gScreenDepth, sampleUV).r;
            if (currentFragDepth == 1.0)  //invalid fragment background 
				break; 

            vec3 FragPos = texture(gWorldPosition, sampleUV).rgb;
            vec3 FragViewPos = (u_view * vec4(FragPos, 1.0)).xyz;
            float FragDepth =  FragViewPos.z;

            //float sampleDepth = texture(gScreenDepth, sampleUV).r; 

            if (abs(currentDepth - FragDepth) < 0.01) // If the depths are close, a reflection is found
            {
                indirColor = texture(u_LightingPassTexture, sampleUV).rgb;
                hit = 1;
                break;
            }
        }


	} 
   // FragColor = vec4(mix(screenColor, indirColor, 0.5), 1.0); // Mix the original color and the reflection
     

    FragColor = vec4(indirColor, 1.0); // Mix the original color and the reflection
    //FragColor = vec4(sampleUV,0.0, 1.0);
    //FragColor = vec4(currentPos,  1.0);
    //FragColor = vec4(vec3(hit), 1.0); 
}
