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



// ------------------------------------------------------------------
// BUFFERS/Images ---------------------------------------------------------
// ------------------------------------------------------------------


layout(rgba32f, binding = 0) uniform readonly image2D u_ViewPosition;
layout(rgba32f, binding = 1) uniform readonly image2D u_ViewNormal;

 
layout(r32f, binding = 2) uniform writeonly image2D u_OutlineMask;


//uniform float u_depth_threshold;

uniform float u_depth_thres ; 
uniform float u_depth_thres_scale;
uniform float u_depth_NdotV_threshold ;

uniform float u_normal_threshold;  

uniform float u_outline_width;

uniform bool u_enable_normal;
uniform bool u_enable_depth;



// ------------------------------------------------------------------

void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imageSize = imageSize(u_ViewPosition);  

    if (pixelCoord.x >= imageSize.x || pixelCoord.y >= imageSize.y) {
        return;
    }
     

    vec3 currentPos = imageLoad(u_ViewPosition, pixelCoord).xyz;
    vec3 currentNormal = imageLoad(u_ViewNormal, pixelCoord).xyz * 2.0f - 1.0f;


    //kernel size by outline width
    //1.0f for 1 pixel-away detection,  
    //so 3.0f we use  2+1 kernel size, by taking floor/ceil of 3.0f/2, 
    // floor for top&right direction, ceil for bottom&left direction 
    
    float outline_width = u_outline_width; 
    float width_floor = floor(outline_width / 2.0f);
    float width_ceil = ceil(outline_width / 2.0f);

    //sample pixelCoords;
    ivec2 top_right = pixelCoord + ivec2(width_ceil, width_ceil);
    ivec2 top_left  = pixelCoord + ivec2(-width_floor, width_ceil);
    ivec2 bottom_right = pixelCoord + ivec2(width_ceil, -width_floor);
    ivec2 bottom_left  = pixelCoord + ivec2(-width_floor, -width_floor);


    //normals
    vec3 TR_normal = imageLoad(u_ViewNormal, top_right).xyz    * 2.0f - 1.0f;
    vec3 TL_normal = imageLoad(u_ViewNormal, top_left).xyz     * 2.0f - 1.0f;
    vec3 BR_normal = imageLoad(u_ViewNormal, bottom_right).xyz * 2.0f - 1.0f;
    vec3 BL_normal = imageLoad(u_ViewNormal, bottom_left).xyz  * 2.0f - 1.0f;

    //the 2-norm of the difference between the normals
    // ||grad|| = sqrt( (n_x - n_x')^2 + (n_y - n_y')^2 + (n_z - n_z')^2 )
    vec3 normal_diff0 = TR_normal.xyz - BL_normal.xyz;
    vec3 normal_diff1 = TL_normal.xyz - BR_normal.xyz;

    float normal_norm = sqrt( dot(normal_diff0, normal_diff0) + dot(normal_diff1, normal_diff1) );


    //same for depth
    float TR_pos = imageLoad(u_ViewPosition, top_right).z;
    float TL_pos = imageLoad(u_ViewPosition, top_left).z;
    float BR_pos = imageLoad(u_ViewPosition, bottom_right).z;
    float BL_pos = imageLoad(u_ViewPosition, bottom_left).z;

    float depth_diff0 = TR_pos - BL_pos;
    float depth_diff1 = TL_pos - BR_pos;

    float depth_norm = sqrt( depth_diff0 * depth_diff0 + depth_diff1 * depth_diff1 ); 





    //---------------------------
    //apply threshold
    float normal_threshold = u_normal_threshold; //0.1;   
    float normal_edge = normal_norm > normal_threshold ? 1.0f : 0.0f;


    //depth is a bit complicated,
    //we want to exclude value from surface depth difference, which takes NdotV into account
    //however,  NdotV near 0 can also be an edge, so what we do is:
    // bias(1+scale),   bias is a numerical bias, scale is 0~0~1  from NdotV 0~thres~ 1

    float NdotV = dot(currentNormal, - normalize(currentPos));
    float inv_NdotV = 1- NdotV;

    float depth_thres = u_depth_thres;       
    float depth_NdotV_threshold = u_depth_NdotV_threshold;     
    float depth_thres_scale = u_depth_thres_scale;         
    //nonlinear mapping by NdotV
    float map_NdotV = clamp(   (inv_NdotV - depth_NdotV_threshold) / (1 - depth_NdotV_threshold), 0.0, 1.0 );
   
    float depth_threshold = depth_thres * (1 + map_NdotV * depth_thres_scale);  

    float depth_edge = depth_norm > depth_threshold ? 1.0f : 0.0f;
     

    //final edge
    if(!u_enable_normal)
        normal_edge = 0.0f;
    if(!u_enable_depth)
        depth_edge = 0.0f;


    //float isEdge = normal_edge * depth_edge; 
    float isEdge = max( normal_edge,  depth_edge);
    //float isEdge = depth_edge; //normal_edge; 

    // Write to the mask texture
    imageStore(u_OutlineMask, pixelCoord, vec4(isEdge, 0.0, 0.0, 1.0) );
}