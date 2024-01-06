#version 460 core

//https ://learnopengl.com/Advanced-Lighting/SSAO
 
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gWorldNormal;
uniform sampler2D gWorldTangent;
//uniform sampler2D gNormal;

uniform sampler2D gScreenDepth;

uniform sampler2D u_NoiseTexture;

uniform vec3 samples[64];

uniform mat4 u_projection;
uniform mat4 u_view; 

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 64;
float radius = 0.2;
float bias = 0.3;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(800.0 / 4.0, 600.0 / 4.0);



void main()
{
    //uniform inputs:
    //both position and normal comes in world;
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gWorldNormal, TexCoords).rgb * 2.0 - 1.0); //from [0,1] to [-1,1]
    vec3 randomVec = normalize(texture(u_NoiseTexture, TexCoords * noiseScale).xyz);

    //the sample vectors are defined in tangent space;
    //create TBN from tangent-space to world space;  //or view space
    //the random vector is to create a random rotation matrix of the tangent space/sample vectors

    //vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    //vec3 bitangent = cross(normal, tangent);
    //mat3 TBN = mat3(tangent, bitangent, normal);


     //turn into view space
     vec3 fragPosView = vec3(u_view * vec4(fragPos, 1.0));
     vec3 normalView = normalize(vec3(u_view * vec4(normal, 0.0)));
      
     //test: use tangent from gbuffer
     vec3 tangent = texture(gWorldTangent, TexCoords).rgb;
     vec3 tangentView = normalize(vec3(u_view * vec4(tangent, 0.0)));

     vec3 bitangentView = cross(normalView, tangentView);
     mat3 TBNView = mat3(tangentView, bitangentView, normalView);



     float occlusionNum = 0.0;
     for (int i = 0; i < kernelSize; ++i)
     {
         //sample position ;  from tangent to world
         vec3 samplePos = TBNView * samples[i];
         samplePos = fragPosView + samplePos * radius;
          
         float sampleDepth = samplePos.z;

         //project to view
         vec4 offset = vec4(samplePos, 1.0); 
         offset = u_projection * offset; // from view to clip-space
         offset.xyz /= offset.w; // perspective divide
         offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

         // get sample depth
         vec3 sampleFragWorldPos = texture(gPosition, offset.xy).rgb;

         //to view
         vec3 sampleFragViewPos = vec3(u_view * vec4(sampleFragWorldPos, 1.0));
         float sampleFragDepth = sampleFragViewPos.z; 

         // range check & accumulate
          float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPosView.z - sampleDepth));
          occlusionNum += (sampleFragDepth >= sampleDepth + bias ? 1.0 : 0.0) * rangeCheck;  
     }

     float occlusion = 1.0 - (occlusionNum / kernelSize);

     FragColor = occlusion;



  

}




 