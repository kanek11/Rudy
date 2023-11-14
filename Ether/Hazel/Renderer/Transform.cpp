#pragma once 
#include "EtherPCH.h"

#include "Transform.h"
#include "Object.h" 
 

namespace Hazel {


    void Transform::UpdateWorldTransformRecursive(const Ref<Transform> node, const glm::mat4& parentTransform)
    {
        //update the global transform;

        HZ_CORE_INFO("transform: to update node: {0}", ( node->gameObject->name ));
        node->worldTransform = parentTransform * (node->localTransform);

        //update the children;
        for (const auto& child : node->children) {

            UpdateWorldTransformRecursive(child, node->worldTransform);

        }

        HZ_CORE_INFO("return");
    }



    uint32_t Transform::GetChildCount() const 
    { return children.size(); }

    void Transform::UpdateLocalTransform()
    {
        localTransform =
            glm::translate(glm::mat4(1.0f), localPosition) *
            glm::mat4_cast(localRotation) *
            glm::scale(glm::mat4(1.0f), localScale);
    }


    void Transform::UpdateWorldTransform()
    {
        worldTransform = 
        glm::translate(glm::mat4(1.0f), position)*
            glm::mat4_cast(rotation)*
            glm::scale(glm::mat4(1.0f), scale); 

    }


    glm::mat4 Transform::GetWorldTransform() const
    { 
        return worldTransform;
    }

}
