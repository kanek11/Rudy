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

        static Ref<Scene> Create();



        std::vector<Ref<Object>> objects;  //scene objects  
         

        void AddObject(Ref<Object> object);
        void Render(Ref<Camera> camera);

        //const std::vector<Ref<Object>>>& get_objects() const { return Objects; }



    
    };


}

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
// 
// 
//camera, lights, etc.
    // 
    //BVHAccel* Bvh = nullptr;  //scene organization if needed

    //void SampleLight(Intersection& pos, float& pdf) const; 
    //void BuildBVH(); 



 