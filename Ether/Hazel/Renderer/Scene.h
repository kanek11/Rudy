 #pragma once

#include "EtherPCH.h"

#include "Object.h"


namespace Hazel {


    class Scene
    {
    public:


        ~Scene() = default;
        Scene() = default;

        std::vector<Ref<Object>> Objects;  //scene objects 


        //std::vector<Object* > Objects;  //scene objects 
        //camera, lights, etc.
        // 
        //BVHAccel* Bvh = nullptr;  //scene organization if needed

        void Add(Ref<Object> object);
         
        //const std::vector<Ref<Object>>>& get_objects() const { return Objects; }


        //void SampleLight(Intersection& pos, float& pdf) const; 
        //void BuildBVH();



    };


}




 