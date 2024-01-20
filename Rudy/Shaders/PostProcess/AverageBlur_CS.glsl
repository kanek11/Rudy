#version 460 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(r32f, binding = 0) uniform image2D inputImage;
layout(r32f, binding = 1) uniform image2D outputImage;

uniform int u_radius;

void main() {

    int radius = u_radius;

    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
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
