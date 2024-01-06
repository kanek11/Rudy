#version 460 core
layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent; 
layout(location = 4) in vec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;

out float test_flag ;



out vec3 WorldPos;
out vec2 TexCoords;
out vec3 WorldNormal;    
out vec3 WorldTangent; 
out mat3 WorldTBN;

uniform mat4 u_model;   //essentially the transform of the root bone
uniform mat4 u_projection;
uniform mat4 u_view;

//the holder of the bone transforms,
//it combines the  ToWorld and ToBoneLocal transforms
uniform mat4 u_BoneTransforms[100];

uniform mat4 u_TestIdentity;

int MAX_BONE_INFLUENCE = 4;

void main()
{
    mat4 boneMatrix = mat4(0.0f);
    for (int i = 0; i < 4; i++)
    { 

        //why??>?? only as float vec4 works?  
        int index = int( aBoneIndices[i] );

        if (index < 100 && index >= 0)
        {
           // boneMatrix  += u_TestIdentity * aBoneWeights[i];
            boneMatrix += u_BoneTransforms[index] * aBoneWeights[i];
        } 
         
      //if (index == -1)
      //{
      //    continue;
      //}
      //
       
      // boneMatrix += u_TestIdentity * aBoneWeights[i];
      // //boneMatrix += mat4(1.0)* aBoneWeights[i];
    }


    //by linearity;
    //mat4 boneMatrix =
    //    u_BoneTransforms[aBoneIndices[0]] * aBoneWeights[0] +
    //    u_BoneTransforms[aBoneIndices[1]] * aBoneWeights[1] +
    //    u_BoneTransforms[aBoneIndices[2]] * aBoneWeights[2] +
    //    u_BoneTransforms[aBoneIndices[3]] * aBoneWeights[3];
    //
    //debug:
     //somthing wrong with weights? say not add to 1.0?
    //boneMatrix =
    // mat4(1.0) * aBoneWeights[0] +
    //  mat4(1.0) * aBoneWeights[1] +
    //  mat4(1.0) * aBoneWeights[2] +
    //  mat4(1.0) * aBoneWeights[3];


    //incorrect transforms?
    //boneMatrix = mat4(1.0);


    WorldPos = vec3( u_model * boneMatrix * vec4(aPos, 1.0) ); 

    //also the normal and tangent need to be transformed
    //the bone animation is basically uniform, so dont need to use the inverse transpose
    WorldNormal =  mat3(u_model) * mat3(boneMatrix) * aNormal;
    WorldTangent = mat3(u_model) * mat3(boneMatrix) * aTangent;
     

    TexCoords = aTexCoords; 

    vec3 WorldBitangent = cross(WorldNormal, WorldTangent);
    WorldTBN = mat3(WorldTangent, WorldBitangent, WorldNormal);

    gl_Position = u_projection * u_view * vec4(WorldPos, 1.0);

}

//if (boneIds[i] == -1)
//continue;
//if (boneIds[i] >= MAX_BONES)
//{
//    totalPosition = vec4(pos, 1.0f);
//    break;
//}