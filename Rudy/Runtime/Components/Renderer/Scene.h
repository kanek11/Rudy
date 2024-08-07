#pragma once
#include "RudyPCH.h"

#include "Object.h"
#include "Camera.h"

namespace Rudy
{

class Scene
{
public:
    ~Scene() = default;
    Scene()  = default;

    static SharedPtr<Scene> Create();

    std::vector<SharedPtr<Object>> objects; // scene objects

    void AddObject(SharedPtr<Object> object);
    void Render(SharedPtr<Camera> camera);

    // const std::vector<SharedPtr<Object>>>& get_objects() const { return Objects; }
};

} // namespace Rudy

// void AddNode(SceneNode* node) {
//     nodes.push_back(node);
// }

//// 更新场景逻辑
// void Update() {
//     // 遍历所有节点并更新它们
//     for (auto node : nodes) {
//         // 更新节点逻辑
//     }
// }
//
//
// camera, lights, etc.
//
// BVHAccel* Bvh = nullptr;  //scene organization if needed

// void SampleLight(Intersection& pos, float& pdf) const;
// void BuildBVH();
