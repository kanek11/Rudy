// Credit to: https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl


#version 460 core

#define GROUP_SIZE_X 8
#define GROUP_SIZE_Y 8 

layout(local_size_x = GROUP_SIZE_X, local_size_y = GROUP_SIZE_Y) in;

layout(rgba32f, binding = 0) uniform image2D inputImage;
layout(rgba32f, binding = 1) uniform image2D outputImage;


uniform float u_exposure;


// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
mat3 ACESInputMat =
{
    {0.59719, 0.35458, 0.04823},
    {0.07600, 0.90834, 0.01566},
    {0.02840, 0.13383, 0.83777}
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
mat3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367},
    {-0.10208,  1.10813, -0.00605},
    {-0.00327, -0.07276,  1.07602}
};

vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

//float3 ACESFitted(float3 color)
//{
//    color = mul(ACESInputMat, color);
//
//    // Apply RRT and ODT
//    color = RRTAndODTFit(color);
//
//    color = mul(ACESOutputMat, color);
//
//    // Clamp to [0, 1]
//    color = saturate(color);
//
//    return color;
//}



void main()
{
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec3 color = u_exposure * imageLoad(inputImage, pixelCoord).rgb;


    if (true)
    { 
        //transpose matrix:   opengl kinda expect store column in rows
    color = transpose(ACESInputMat) * color;
    color = RRTAndODTFit(color);
    color = transpose(ACESOutputMat) * color;
    }

    //color = color / (color + vec3(1.0f));

    //gamma encoding if needed
    //color = pow(color, vec3(1.0f / 2.2f));
    color = clamp(color, 0.0, 1.0); 
   
    imageStore(outputImage, pixelCoord, vec4(color, 1.0));

}