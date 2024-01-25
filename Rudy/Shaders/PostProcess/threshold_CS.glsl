#version 460 core
#define GROUP_SIZE_X 8
#define GROUP_SIZE_Y 8 

layout(local_size_x = GROUP_SIZE_X, local_size_y = GROUP_SIZE_Y) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputImage;
layout(rgba32f, binding = 1) uniform writeonly image2D outputImage;


uniform float u_threshold;


void main() {

    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec4 pixelColor = imageLoad(inputImage, pixelCoord);

    //intensity
    float brightness = dot(pixelColor.rgb, vec3(0.299, 0.587, 0.114));

    if (brightness < u_threshold) {
        pixelColor = vec4(0, 0, 0, 1); // set as black
    }
     
    imageStore(outputImage, pixelCoord, pixelColor);
}