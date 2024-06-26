#version 460 core

layout(local_size_x = 8, local_size_y = 8) in;

layout(r32f, binding = 0) uniform image2D inputImage;
layout(r32f, binding = 1) uniform image2D outputImage; 

 
const float sigma = 5.0;
const int radius = 4;
const int size = (2 * radius + 1) * (2 * radius + 1);

float weight[size];
ivec2 offset[size];


//room for improvement: 
// use two passes for the horizontal and vertical blur
// use shared memory to store the weights and offsets
// only compute half of the weights and offsets, and then mirror them
// use a CPU 1D texture to store the weights and offsets
void calculateWeights() {
    float sum = 0.0;
    int i = 0;
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            float w = exp(-(x * x + y * y) / (2 * sigma * sigma));
            weight[i] = w;
            offset[i] = ivec2(x, y);
            sum += w;
            i++;
        }
    }

    // Normalize weights
    for (int j = 0; j < size; j++) {
        weight[j] /= sum;
    }

}

void main() {


    calculateWeights();

    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec4 sum = vec4(0.0);

    for (int i = 0; i < size; i++) {
        sum += imageLoad(inputImage, pixelCoord + offset[i]) * weight[i];
    }

    imageStore(outputImage, pixelCoord, sum);

}
