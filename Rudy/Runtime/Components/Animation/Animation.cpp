#include "RudyPCH.h"
#include "Animation.h"

#include "Runtime/Components/Renderer/Object.h"

namespace Rudy
{

SharedPtr<AnimationClip> AnimationClip::Create()
{
    return CreateShared<AnimationClip>();
}

AnimationClip::AnimationClip()
{
}

// interpolation goes here;  interpolate update local transform;  then update global transform;
void AnimationClip::CalculateKeyframe(float time)
{
    // into frame index;
    // the casted frame index is the floor;
    // so find nearest frame index is to
    //  before =  largest index <= floor;  after = if > floor;
    //  and since each properties can be keyed in different frame index; need to search for each;

    // time to ticks;
    uint32_t floor_frameIndex = static_cast<int>(time * FPS);
    // RD_CORE_INFO("animationclip: the floor frame index is {0}", floor_frameIndex);

    // iterate through each keybone;
    for (const auto& keybonePair : keybones)
    {
        // KeyframeBone newKeyframeBone = KeyframeBone();

        const std::string& keyboneName      = keybonePair.first;
        const auto&        keybone          = keybonePair.second;
        auto               keyboneTransform = keybone->m_Node;

        // find the nearest frame index;
        int before_frameIndex = -1;
        int after_frameIndex  = -1;

        auto before_position = glm::vec3(0.0f);
        auto after_position  = glm::vec3(0.0f);

        for (const auto& keyPosition : keybone->m_KeyPositions)
        {
            // search for the nearest frame index; todo: more efficient search;
            if (keyPosition.frameIndex <= floor_frameIndex)
            {
                before_frameIndex = keyPosition.frameIndex;
                before_position   = keyPosition.position;
            }
            else
            {
                after_frameIndex = keyPosition.frameIndex;
                after_position   = keyPosition.position;
                break;
            }
        }

        // interpolate the position;
        float lerpFactor = (time * FPS - before_frameIndex) / (after_frameIndex - before_frameIndex);
        // wrong: glm::vec3 localPosition = glm::mix(keybone->m_KeyPositions[before_frameIndex].position, keybone->m_KeyPositions[after_frameIndex].position, lerpFactor);
        glm::vec3 localPosition = glm::mix(before_position, after_position, lerpFactor);

        auto before_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        auto after_rotation  = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        for (const auto& keyRotation : keybone->m_KeyRotations)
        {
            if (keyRotation.frameIndex <= floor_frameIndex)
            {
                before_frameIndex = keyRotation.frameIndex;
                before_rotation   = keyRotation.rotation;
            }
            else
            {
                after_frameIndex = keyRotation.frameIndex;
                after_rotation   = keyRotation.rotation;
                break;
            }
        }

        // interpolate the rotation; quaternion use slerp;
        lerpFactor              = (time * FPS - before_frameIndex) / (after_frameIndex - before_frameIndex);
        glm::quat localRotation = glm::slerp(before_rotation, after_rotation, lerpFactor);

        auto before_scale = glm::vec3(1.0f);
        auto after_scale  = glm::vec3(1.0f);

        for (const auto& keyScale : keybone->m_KeyScales)
        {
            if (keyScale.frameIndex <= floor_frameIndex)
            {
                before_frameIndex = keyScale.frameIndex;
                before_scale      = keyScale.scale;
            }
            else
            {
                after_frameIndex = keyScale.frameIndex;
                after_scale      = keyScale.scale;
                break;
            }
        }

        // interpolate the scale;
        lerpFactor           = (time * FPS - before_frameIndex) / (after_frameIndex - before_frameIndex);
        glm::vec3 localScale = glm::mix(before_scale, after_scale, lerpFactor);

        // debug: leave it be;
        bool _update = true;
        // set the local transform;
        if (_update)
        {
            keyboneTransform->localPosition = localPosition;
            keyboneTransform->localRotation = localRotation;
            keyboneTransform->localScale    = localScale;
            keyboneTransform->UpdateLocalTransform();
        }

        // if use C# property ,this update can be implemented in the setter;?
    }

    // update the global transform ,by traverse from root to leaf;
    // todo : control transform of root node;
    // UpdateGlobalTransformRecursive(this->rootNode, rootNode->GetWorldTransform());
    SceneComponent::UpdateWorldTransformRecursive(this->rootNode, glm::mat4(1.0f));
}

glm::mat4 Animator::GetGlobalTransform(const std::string& name)
{
    auto _currentKeyBones = this->animationClip->keybones;
    // debug:
    if (_currentKeyBones.find(name) == _currentKeyBones.end())
        RD_CORE_INFO("animclip: bone {0} is not found as keyed properties", name);

    // find the node by name in the model;
    auto _node = this->model->findNode(name);

    return _node->transform->worldTransform;
}

//=============

// get the data-oriented array from the avatar;
//
Animator::Animator(SharedPtr<Model> model) :
    model(model)
{
    // m_BoneTransforms.resize(100);  //TODO: adjust the size from the avatar;
}

std::vector<glm::mat4> Animator::GetBoneTransforms()
{
    return m_BoneTransforms;
}

// basically forward kinematics; traverse the hierarchy;
void Animator::UpdateBoneTransforms(float time)
{
    // simple loop mode;
    if (!this->animationClip)
    {
        RD_CORE_ERROR("animator: no animation found");
        return;
    }

    auto duration = this->animationClip->duration;
    auto FPS      = this->animationClip->FPS;
    time          = fmod(time, duration / FPS);

    // update ,then get;
    // calculate the global transform first; this ensures go from root to leaf; not by index order;
    this->animationClip->CalculateKeyframe(time);

    for (const auto& bonePair : model->boneMap)
    {
        const std::string& boneName = bonePair.first;
        auto               boneID   = bonePair.second;

        glm::mat4 modelToLocal = model->bindPose[boneID];

        // the default
        glm::mat4 animLocalToGlobal = glm::inverse(modelToLocal);

        // get info from the current keyframe; by name;
        animLocalToGlobal = this->GetGlobalTransform(boneName);

        ////debug: print out the transform;
        // std::cout << "name: " << boneName << ", id: " << bone.boneIndex << std::endl;
        //  for (int i = 0; i < 4; i++) {
        //	 for (int j = 0; j < 4; j++) {
        //		 std::cout << animLocalToGlobal[i][j] << " ";
        //	 }
        //	 std::cout << std::endl;
        //  }

        // test for the shader, pass identity;
        m_BoneTransforms[boneID] = animLocalToGlobal * modelToLocal;
        // m_BoneTransforms[bone.boneIndex] = glm::mat4(1.0f);
        // RD_CORE_INFO("animatior: set bone:{0}", bone.boneIndex);

        // the offset to transform from bone space to model space;
    }
    // SetBoneTransformsRecursive(time, m_AnimationClip->rootNode, glm::mat4(1.0f));
}

void AnimationClip::printHierarchy(const SharedPtr<SceneComponent> node)
{
    RD_CORE_INFO("animationclip print: node name:{0}", node->gameObject->name);

    for (const auto& child : node->children)
    {
        printHierarchy(child);
    }

    RD_CORE_INFO("return");
}

} // namespace Rudy