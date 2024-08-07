#pragma once

#include <glm/gtx/quaternion.hpp>

#include "Runtime/Components/Renderer/Transform.h"
#include "RudyPCH.h"

// animator is created and linked to a model ;
// manually set animator to model, and link the animtion clips.

// an animator basically  composed of two parts:
// for character animation,
// staic£º bone data, in "bind pose" or "rest pose";
// dynamic:  the manipulation logic;  that drives the bone transforms;

// the goal API is : GetBoneTransforms(index);  for the vertex shader to use;

// for time-locality, we keep a sequence of keyframes ;
// not seperate time sequence of each bone;

// todo: out of range wrapping behavior;

namespace Rudy
{

// for now:
class Model;

// determined id by traverse the aiBone traversed in the aiMesh;
// correspond to a  boneTransforms[] passed to shader;
// struct Bone
//{
//    int boneIndex;
//    glm::mat4 bindPoseTransform;
//};

// defines the keyed properties of a bone transform;
struct KeyPosition
{
    glm::vec3 position;
    uint32_t  frameIndex;
};

struct KeyRotation
{
    glm::quat rotation;
    uint32_t  frameIndex;
};

struct KeyScale
{
    glm::vec3 scale;
    uint32_t  frameIndex;
};

// the keyed bone data of the animation clip;
// think of it as "row" of the dope sheet;
struct KeyBone
{
    // for static rest-pose;
    SharedPtr<Transform> m_Node = CreateShared<Transform>();

    std::vector<KeyPosition> m_KeyPositions;
    std::vector<KeyRotation> m_KeyRotations;
    std::vector<KeyScale>    m_KeyScales;
};

// key frame for dynamic animation;
// struct KeyframeBone {
//
//	//refer to gameobject in the scene;
//	 SharedPtr<Transform> m_Node = CreateShared<Transform>();
//
// };
//
//
////runtime data of the animation clip;
////think of it as "column" of the dope sheet;
// struct Keyframe {
//
//	std::unordered_map<std::string, KeyframeBone> keyframebones;
//
// };

// time =  frame_index * duration / FPS;
// frame_index  = time * FPS / duration;

class AnimationClip
{
public:
    ~AnimationClip() = default;
    AnimationClip();
    static SharedPtr<AnimationClip> Create();

    float    duration = 0; // in seconds;
    uint32_t FPS      = 0;

    // keyed properties;
    // the animation clip keeps a list of keyframes for each animated property;
    std::unordered_map<std::string, SharedPtr<KeyBone>> keybones;

    // same as model root;
    SharedPtr<Transform> rootNode = CreateShared<Transform>();

    // Keyframe m_CurrentKeyframe = Keyframe();

    // TODO: make the property generic if we have reflection;
    void AddKeyBone(const std::string& name, const SharedPtr<KeyBone> keybone)
    {
        keybones[name] = keybone;
    }

    // update befor get;
    void CalculateKeyframe(float time);

    void printHierarchy(const SharedPtr<Transform> node);
};

// the animator drives the animation;
// whatever the logic is , it returns the data-orientated bone transform[]; that
// is sent to GPU;

// it refer to
//  a controller to manipulate the animationClip;  //just simple clip for now;
//  the bone data of animated model;

// todo: multiple animation clips;
class Animator
{
public:
    ~Animator() = default;
    Animator(SharedPtr<Model> model);

    static SharedPtr<Animator> Create(SharedPtr<Model> model = nullptr)
    {
        return CreateShared<Animator>(model);
    }

    void                   UpdateBoneTransforms(float time);
    std::vector<glm::mat4> GetBoneTransforms();

    // scene
    glm::mat4 GetGlobalTransform(const std::string& name);

    // as engine asset;
    SharedPtr<AnimationClip> animationClip = nullptr;

    // in the model; todo:  make it for gameobject instead of model asset;
    SharedPtr<Model> model = nullptr;

    // std::map<std::string, Bone> bindPoseBoneMap;
    // SharedPtr<Object> rootNode;

    std::vector<glm::mat4> m_BoneTransforms = std::vector<glm::mat4>(100, glm::mat4(1.0f));
};

} // namespace Rudy