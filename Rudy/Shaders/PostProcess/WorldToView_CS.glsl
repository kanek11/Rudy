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

uniform mat4 u_view; 

// ------------------------------------------------------------------
// BUFFERS/Images ---------------------------------------------------------
// ------------------------------------------------------------------


layout(binding = 0, rgba32f) uniform readonly image2D u_WorldPosition;
layout(binding = 1, rgba32f) uniform readonly image2D u_WorldNormal;
//layout(binding = 2, r32f) uniform readonly image2D u_ViewDepth;
layout(binding = 0) uniform sampler2D u_ViewDepth;


layout(binding = 3, rgba32f) uniform writeonly image2D u_ViewPosition;
layout(binding = 4, rgba32f) uniform writeonly image2D u_ViewNormal; 


void main()
{ 
	ivec2 pixel_index = ivec2(gl_GlobalInvocationID.xy);
	if (pixel_index.x >= imageSize(u_WorldPosition).x || pixel_index.y >= imageSize(u_WorldPosition).y)
	{
		return; 
	}

	vec2 uv = vec2(pixel_index +0.5) / vec2(imageSize(u_WorldPosition));

	 
	vec4 worldPos =    imageLoad(u_WorldPosition, pixel_index);
	vec4 worldNormal = imageLoad(u_WorldNormal, pixel_index) * 2.0 - 1.0;  //convert from [0,1] to [-1,1]
	
	//handle the affine tran correctly
	vec4 viewPos =    u_view * vec4(worldPos.xyz,1.0);
	vec4 viewNormal = vec4( mat3(u_view)* worldNormal.xyz ,1.0);


	if (texture(u_ViewDepth, uv).x == 1.0)
	{
		//donot return here, but write the pixel with the background color
		viewPos = vec4(0.0, 0.0, 0.0, 1.0);
		viewNormal =  vec4(0.0, 0.0, 0.0, 1.0);
	}
	
	imageStore(u_ViewPosition, pixel_index, viewPos);
	imageStore(u_ViewNormal, pixel_index, viewNormal * 0.5 + 0.5);  //convert from [-1,1] to [0,1]

}
