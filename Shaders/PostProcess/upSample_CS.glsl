#version 460 core

#define GROUP_SIZE  8
#define MAX_RADIUS  12 

layout(local_size_x = GROUP_SIZE, local_size_y = GROUP_SIZE) in;

layout(binding = 0) uniform sampler2D input_sampler;
layout(rgba32f, binding = 0) uniform image2D output_image;

uniform int u_kernel_radius;
uniform vec2 u_direction; // horizontal or vertical 

//paramaters
//global scalar
uniform float u_bloom_strength;
//to scale each level
uniform float u_bloom_radius;
uniform float u_factor;


// Declare as global variable, size is known at compile time
float weight[2 * MAX_RADIUS + 1];

void calculateWeights(int radius, float sigma) {
    float sum = 0.0;
    int i = 0;
    for (int x = -radius; x <= radius; x++) {
        float w = exp(-(x * x) / (2 * sigma * sigma));
        weight[i] = w;
        sum += w;
        i++;
    }

    // Normalize weights
    for (int j = 0; j < 2 * radius + 1; j++) {
        weight[j] /= sum;
    }
}



float lerpBloomFactor(const in float factor) {
    float mirrorFactor = 1.2 - factor;
    return mix(factor, mirrorFactor, u_bloom_radius);
}



void main() {
    calculateWeights(u_kernel_radius, 5.0);

    int radius = u_kernel_radius;

    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID);
    vec2 inv_size = vec2(1.0) / vec2(imageSize(output_image));
    vec2 base_uv = (pixel_coords + 0.5) * inv_size;

    vec4 color_sum = vec4(0.0);

    for (int i = -radius; i <= radius; i++) {
        vec2 offset = u_direction * float(i) * inv_size;
        color_sum += texture(input_sampler, base_uv + offset) * weight[radius + i];
    }

    //add back itself on the level , note: only do this for vertical pass
    if (u_direction.y == 1.0) {
        vec4 this_color = imageLoad(output_image, pixel_coords);
        color_sum += this_color * lerpBloomFactor(u_factor);
        color_sum *= u_bloom_strength; 
	}
    else
    { 
		color_sum *= u_bloom_strength; 
    }

    //color_sum = vec4(1.0, 1.0, 1.0, 1.0); //debug
    imageStore(output_image, pixel_coords, color_sum); 
}
