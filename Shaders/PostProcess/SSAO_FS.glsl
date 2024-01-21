#version 460 core

layout(location = 0) out float out_FragColor;

in vec2 TexCoords;



uniform sampler2D gViewPosition;
uniform sampler2D gViewNormal;

uniform sampler2D gWorldTangent;
uniform sampler2D gViewDepth;
 
uniform sampler2D u_NoiseTexture;

uniform vec3 samples[64];

uniform mat4 u_view;
uniform mat4 u_projection;



// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
uniform int u_kernelSize = 64;
uniform float u_radius ;
uniform float u_bias ;

uniform float u_range_bias;


// tile noise texture over screen based on screen dimensions divided by noise size
// not have to be based on actual screen size;
uint width = 800;
uint height = 600;
const vec2 noiseScale = vec2(width / 4.0, height / 4.0);



void main()
{
    //ignore background fragments
    float depth = texture(gViewDepth, TexCoords).r;
    if (depth == 1.0) //if the depth is 1.0, it's outside the screen
    {
        out_FragColor = 1.0;  return;  //discard if want it to be black
    } 
  
    //uniform inputs:
    //both position and normal comes in world;
    vec3 ViewFragPos = texture(gViewPosition, TexCoords).rgb; 
    vec3 ViewNormal = normalize( texture(gViewNormal, TexCoords).rgb * 2.0 - 1.0); //from [0,1] to [-1,1]

  
    //adjust the bias ; camera is at the origin
    vec3 viewDir = normalize(vec3(0.0f) - ViewFragPos);
    //bias = max(u_max_bias * (1.0 - dot( ViewNormal, viewDir)), u_min_bias);
    float bias = u_bias;
    
    //the sample vectors are defined in tangent space;
    //create TBN from tangent-space to view space;  
   //the random vector is to create a random rotation matrix of the tangent space/sample vectors

    vec3 randomVec = normalize( texture(u_NoiseTexture, TexCoords * noiseScale).xyz);
    //exclude normal component by gram-schmidt process
    vec3 ViewTangent = normalize(randomVec - ViewNormal * dot(randomVec, ViewNormal));
    
    //test: use tangent from gbuffer
    // vec3 tangent = texture(gWorldTangent, TexCoords).rgb;
     //vec3 ViewTangent = normalize(vec3(u_view * vec4(tangent, 0.0)));

     vec3 ViewBitangent = normalize( cross(ViewNormal, ViewTangent) );
     mat3 ViewTBN= mat3(ViewTangent, ViewBitangent, ViewNormal);



     float occlusionNum = 0.0; 
     for (int i = 0; i < u_kernelSize; ++i)
     {
         //sample position ;  from tangent to world
         vec3 sampleOffset = ViewTBN * samples[i];
         vec3 samplePos = ViewFragPos + sampleOffset * u_radius;
          
         float sampleDepth = samplePos.z;

         //get the depth of fragment
         //project to view 
         vec4 sampleNDC = u_projection * vec4(samplePos, 1.0); // from view to clip-space
         sampleNDC.xyz /= sampleNDC.w; // perspective divide
         sampleNDC.xyz = sampleNDC.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

         //ignore samples that are outside the screen
         float sampleScreenZ = texture(gViewDepth, sampleNDC.xy).r; //depth from screen space
         if (sampleScreenZ == 1.0) //if the depth is 1.0, it's outside the scene
         { 
             continue;
         }
         // 
         vec3 sampleFragPos = texture(gViewPosition, sampleNDC.xy).rgb;
         float sampleFragDepth = sampleFragPos.z;

         //testOutput = samples[i].x;
          // range check & accumulate
         //float rangeCheck = smoothstep(0.0, 1.0, u_radius / abs(ViewFragPos.z - sampleFragDepth));
        
   
         //float rangeCheck = smoothstep(0.0, 1.0, length(samples[i]) / length(ViewFragPos - sampleFragPos));
         float rangeCheck = step(1.0 - u_range_bias, length(samples[i]) / length(ViewFragPos - sampleFragPos));

         occlusionNum += (sampleFragDepth > sampleDepth + bias ? 1.0 : 0.0)  * rangeCheck;
     
     }


     float occlusion = 1.0 - (occlusionNum / u_kernelSize);

     out_FragColor = occlusion;


     //testOutput = fragPos.z; 
     //FragColor = 0.5;
  

}




 