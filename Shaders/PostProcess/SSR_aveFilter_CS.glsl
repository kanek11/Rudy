#version 460 core

#define GROUP_SIZE_X 8
#define GROUP_SIZE_Y 8 

layout(local_size_x = GROUP_SIZE_X, local_size_y = GROUP_SIZE_Y) in;

layout(rgba32f, binding = 0) uniform readonly image2D u_inputImage;
layout(r32f, binding = 1) uniform readonly image2D u_roughness; 

layout(rgba32f, binding = 2) uniform writeonly image2D u_outputImage; 



void main() {

    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy); 

    //radius by roughness
    float max_radius = 10;
    int radius =  int(imageLoad(u_roughness, pixelCoord).r * max_radius) ;

    vec3 sum = vec3(0.0);
    int count = 0;

    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            ivec2 offset = ivec2(x, y);
            sum += imageLoad(u_inputImage, pixelCoord + offset).xyz;
            count++;
        }
    }

    vec3 average = sum / float(count);
    imageStore(u_outputImage, pixelCoord, vec4(average,1.0));
}
