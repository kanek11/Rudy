#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
//bitangent to be compute by cross product; this saves a buffer slot

layout(location = 4) in vec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;



out VS_OUT
{
    vec3 WorldPos;
    vec2 TexCoords;
    vec3 WorldNormal;
    vec3 WorldTangent;
    mat3 WorldTBN;

} vs_out;



uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;


//new to skinned mesh , only change the vertex shader
//the holder of the bone transforms,
//it combines the  ToWorld and ToBoneLocal transforms
uniform mat4 u_BoneTransforms[100]; 

int MAX_BONE_INFLUENCE = 4;

void main()
{
    mat4 boneMatrix = mat4(1.0f);
    for (int i = 0; i < 4; i++)
    {
        //why??>?? only as float vec4&cast works?  ivec4 not work? maybe bit length?
        int index = int(aBoneIndices[i]);

        if (index < 100 && index >= 0)
        {
            // boneMatrix  += u_TestIdentity * aBoneWeights[i];
            boneMatrix += u_BoneTransforms[index] * aBoneWeights[i];
        }

    }



    mat4 model = u_model * boneMatrix;  

    vs_out.TexCoords = aTexCoords;
    vs_out.WorldNormal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.WorldTangent = mat3(transpose(inverse(model))) * aTangent;


    vs_out.WorldPos = vec3(model * vec4(aPos, 1.0));

    //warning: make sure the handedness is correct
    vec3 WorldBitangent = cross(vs_out.WorldNormal, vs_out.WorldTangent);
    vs_out.WorldTBN = mat3(vs_out.WorldTangent, WorldBitangent, vs_out.WorldNormal);

    gl_Position = u_projection * u_view * vec4(vs_out.WorldPos, 1.0);

}