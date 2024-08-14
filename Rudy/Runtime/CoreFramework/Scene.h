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

//// ���³����߼�
// void Update() {
//     // �������нڵ㲢��������
//     for (auto node : nodes) {
//         // ���½ڵ��߼�
//     }
// }
//
//
// camera, lights, etc.
//
// BVHAccel* Bvh = nullptr;  //scene organization if needed

// void SampleLight(Intersection& pos, float& pdf) const;
// void BuildBVH();
