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

    //// ���³����߼�
    //void Update() {
    //    // �������нڵ㲢��������
    //    for (auto node : nodes) {
    //        // ���½ڵ��߼�
    //    }
    //}
}




 