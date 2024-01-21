#pragma once 
#include "RudyPCH.h"
 
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

//https://docs.unity3d.com/2023.3/Documentation/ScriptReference/Transform.html
// 
//the transform is a essential component of a game object,
//consider that all game object have position, rotation and scale
// 
//and it also serve the role of node in the scene hierarchy

//glm quaternion :
//https://glm.g-truc.net/0.9.0/api/a00135.html

 

namespace Rudy {


    //declare the class, dont declare out the namespace
    class Object;

    class Transform
    {
     public:
        //world ;
        //won't expose to user; because it's possible to have it un-synced with this->transform;
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); 
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  //wxyz  
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);


        //local transform , relative to parent;  exposed to user;  
        glm::vec3 localPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  //wxyz  
        glm::vec3 localScale = glm::vec3(1.0f, 1.0f, 1.0f);


        //system implementation
        //transform as a whole affine matrix  
        glm::mat4 localTransform = glm::mat4(1.0f);
        glm::mat4 worldTransform = glm::mat4(1.0f);


        //the hierarchy 
        Ref<Transform> parent;
        std::vector<Ref<Transform>> children;

        uint32_t GetChildCount() const;
        glm::mat4 GetWorldTransform();


 public:
        void UpdateLocalTransform(); 
        void UpdateWorldTransform();

        static void UpdateWorldTransformRecursive(const Ref<Transform> node, const glm::mat4& parentTransform);
    
public: 

       //in turn reference to the game object
       Ref<Object> gameObject;  
     
    };




 
















}