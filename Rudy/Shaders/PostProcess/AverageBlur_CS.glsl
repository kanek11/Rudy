#version 460 core

#define GROUP_SIZE_X 8
#define GROUP_SIZE_Y 8 

layout(local_size_x = GROUP_SIZE_X, local_size_y = GROUP_SIZE_Y) in;

layout(r32f, binding = 0) uniform image2D inputImage;
layout(r32f, binding = 1) uniform image2D outputImage;



uniform int u_radius;

void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);


    int radius = u_radius;

     vec4 sum = vec4(0.0);
    int count = 0;

    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            ivec2 offset = ivec2(x, y);
            sum += imageLoad(inputImage, pixelCoord + offset);
            count++;
        }
    }

    vec4 average = sum / float(count);
    imageStore(outputImage, pixelCoord, average);
}
