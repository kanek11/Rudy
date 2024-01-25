#version 460 core

#define GROUP_SIZE_X 8
#define GROUP_SIZE_Y 8 

layout(local_size_x = GROUP_SIZE_X, local_size_y = GROUP_SIZE_Y) in;

layout(rgba32f, binding = 0) uniform image2D LitOutput;
layout(rgba32f, binding = 1) uniform readonly image2D SSROutput;

layout(r32f, binding = 2) uniform readonly image2D OutlineOutput; 

//layout(rgba32f, binding = 3) uniform writeonly image2D outputImage;


uniform float u_outline_scale;
uniform float u_SSR_scale;




void main() {


	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

	vec3 litColor = imageLoad(LitOutput, pixelCoords).rgb;
	vec3 ssrColor = imageLoad(SSROutput, pixelCoords).rgb;

	float outlineColor = imageLoad(OutlineOutput, pixelCoords).r; 

	vec3 finalColor = litColor * (1 - outlineColor * u_outline_scale) + u_SSR_scale * ssrColor ;

	imageStore(LitOutput, pixelCoords, vec4(finalColor,1.0));

}