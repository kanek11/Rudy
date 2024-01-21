#version 460 core

// ------------------------------------------------------------------
// CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------


#define GROUP_SIZE_X 8
#define GROUP_SIZE_Y 8


// ------------------------------------------------------------------
// INPUTS -----------------------------------------------------------
// ------------------------------------------------------------------


layout(local_size_x = GROUP_SIZE_X, local_size_y = GROUP_SIZE_Y, local_size_z = 1) in;


// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------


uniform mat4 u_projection;

uniform float u_depth_bias;

uniform float u_step_size;
uniform int   u_max_steps;



// ------------------------------------------------------------------
// BUFFERS/Images ---------------------------------------------------------
// ------------------------------------------------------------------


layout(binding = 0) uniform sampler2D u_ViewPosition;
layout(binding = 1) uniform sampler2D u_ViewNormal;
layout(binding = 2) uniform sampler2D u_LitTexture;

layout(binding = 3) uniform sampler2D u_ViewDepth; //32bit?


layout(rgba32f, binding = 0) uniform writeonly image2D u_SSROutput;



void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv = vec2(pixelCoord + 0.5) / vec2(imageSize(u_SSROutput));

    vec3 normal =   texture(u_ViewNormal,   uv).xyz * 2.0 - 1.0;
    vec3 position = texture(u_ViewPosition, uv).xyz;
    float depth =   texture(u_ViewDepth,  uv).x;

    //view direction
    vec3 viewDir = normalize(position);

    //reflection vector, accepts viewDir as it is from camera 
    vec3 reflectDir = reflect(viewDir, normal);


    //==================================================================
    //raymarching
    int max_steps = u_max_steps; // 100;
    float step_size = u_step_size; // 0.1;

    float depth_bias = u_depth_bias;  //   0.01f;

    vec3 currentPos = position + reflectDir * 0.01f; //
    vec3 reflectedColor = vec3(0.0);
    bool hit = false;
    for (int i = 0; i < max_steps; i++) {

        currentPos += reflectDir * step_size; 

        vec4 clipCoords = u_projection * vec4(currentPos, 1.0f);
        vec3 NDC = clipCoords.xyz / clipCoords.w;

        //convert to texture coordinates
        vec2 texCoords = NDC.xy * 0.5f + 0.5f;
        if (texCoords.x < 0.0 || texCoords.x > 1.0 || texCoords.y < 0.0 || texCoords.y > 1.0)
        {
            break;
        }

        //sample screen depth
        float sampleViewDepth = texture(u_ViewDepth, texCoords).x;
        if (sampleViewDepth ==1.0f) {
            break;  //out of scene
		}

        vec3 sampleFragPos = texture(u_ViewPosition, texCoords).xyz;
        float sampleFragDepth = sampleFragPos.z;

        if (abs(sampleFragDepth - currentPos.z) < depth_bias) {
            hit = true;
            reflectedColor = texture(u_LitTexture, texCoords).rgb;
            hit = true;
            break;
        }
         
	}

  
	imageStore(u_SSROutput, pixelCoord, vec4(reflectedColor,1.0));
   // imageStore(u_SSRResult, pixelCoord, hit ? reflectedColor : color);


}