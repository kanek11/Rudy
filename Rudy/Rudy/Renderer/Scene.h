 #pragma once

#include "RudyPCH.h"
 
#include "Object.h"
#include "Rudy/Renderer/Light.h"
#include "Rudy/Renderer/Camera.h" 


namespace Rudy {


    class Scene
    {
    public:
        ~Scene() = default;
        Scene() = default;

        std::vector<Ref<RenderableObject>> renderObjects;  //scene objects  
         

        static Ref<Scene> Create();
        //camera, lights, etc.
        // 
        //BVHAccel* Bvh = nullptr;  //scene organization if needed

        void AddRenderableObject(Ref<RenderableObject> object);
        void Render(Ref<Camera> camera);

        //const std::vector<Ref<Object>>>& get_objects() const { return Objects; }


        //void SampleLight(Intersection& pos, float& pdf) const; 
        //void BuildBVH(); 

    };

    //void AddNode(SceneNode* node) {
    //    nodes.push_back(node);
    //}

    //// 更新场景逻辑
    //void Update() {
    //    // 遍历所有节点并更新它们
    //    for (auto node : nodes) {
    //        // 更新节点逻辑
    //    }
    //}
}




 