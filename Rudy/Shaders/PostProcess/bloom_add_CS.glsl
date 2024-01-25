#version 460 core
#define GROUP_SIZE_X 8
#define GROUP_SIZE_Y 8 

layout(local_size_x = GROUP_SIZE_X, local_size_y = GROUP_SIZE_Y) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputImage;
layout(rgba32f, binding = 1) uniform image2D outputImage;



void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);


    vec3 bloom = imageLoad(inputImage, pixelCoord).rgb;
    vec3 original = imageLoad(outputImage, pixelCoord).rgb;

    vec3 result = original + bloom;

    imageStore(outputImage, pixelCoord, vec4(result, 1.0));  
}
