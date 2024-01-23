#version 460 core
layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent; 
layout(location = 4) in vec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;


uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view; 

 
int MAX_BONE_INFLUENCE = 4;


layout(std430, binding = 0) buffer BoneTransforms_t {
    mat4 BoneTransforms[100];
};


void main()
{

    mat4 boneMatrix = mat4(0.0f);
    for (int i = 0; i < 4; i++)
    { 
        int index = int(aBoneIndices[i]);

        if (index < 100 && index >= 0)
        { 
            boneMatrix +=  BoneTransforms[index] * aBoneWeights[i];
        }
         
    } 
    gl_Position = u_projection * u_view * u_model * boneMatrix * vec4(aPos, 1.0);

}