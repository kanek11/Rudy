#pragma once
#include "RudyPCH.h"

#include "SceneComponent.h"

namespace Rudy
{

USceneComponent::USceneComponent() :
    UActorComponent()
{
}

void USceneComponent::TickComponent(float DeltaTime)
{
    // call parent tick
    UActorComponent::TickComponent(DeltaTime);
    if (m_isLocalDirty)
    {
        RecalcWorldTransformHierarchy();
        m_isLocalDirty = false;
    }
}

uint32_t USceneComponent::GetChildrenCount() const
{
    return static_cast<uint32_t>(m_children.size());
}

void USceneComponent::SetLocalPosition(const glm::vec3& _position)
{
    // std::cout << "set local position: " << _position << std::endl;
    this->localPosition  = _position;
    this->m_isLocalDirty = true;
}

void USceneComponent::SetLocalRotation(const glm::quat& rotation)
{
    this->localRotation  = rotation;
    this->m_isLocalDirty = true;
}

void USceneComponent::SetLocalScale(const glm::vec3& scale)
{
    this->localScale     = scale;
    this->m_isLocalDirty = true;
}

void USceneComponent::RecalcLocalTransform()
{
    this->localTransformMatrix = glm::translate(glm::mat4(1.0f), localPosition)
        * glm::mat4_cast(localRotation)
        * glm::scale(glm::mat4(1.0f), localScale);
}

glm::vec3 USceneComponent::GetWorldPosition() const
{
    return glm::vec3(this->worldTransformMatrix[3]);
}

void USceneComponent::SetWorldPosition(const glm::vec3& position)
{
    if (m_parent.expired())
    {
        this->localPosition  = position;
        this->m_isLocalDirty = true;
    }
    else
    {
        auto parent          = m_parent.lock();
        this->localPosition  = glm::vec3(glm::inverse(parent->GetWorldTransform()) * glm::vec4(position, 1.0f));
        this->m_isLocalDirty = true;
    }
}

glm::quat USceneComponent::GetWorldRotation() const
{
    return glm::quat_cast(this->worldTransformMatrix);
}

void USceneComponent::SetWorldRotation(const glm::quat& rotation)
{
    if (m_parent.expired())
    {
        SetLocalRotation(rotation);
    }
    else
    {
        auto parent = m_parent.lock();
        SetLocalRotation(glm::inverse(parent->GetWorldRotation()) * rotation);
    }
}

void USceneComponent::SetWorldScale(const glm::vec3& scale)
{
    if (m_parent.expired())
    {
        this->localScale     = scale;
        this->m_isLocalDirty = true;
    }
    else
    {
        auto parent          = m_parent.lock();
        this->localScale     = scale / parent->GetWorldScale();
        this->m_isLocalDirty = true;
    }
}

glm::vec3 USceneComponent::GetWorldScale() const
{
    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(this->worldTransformMatrix[0]));
    scale.y = glm::length(glm::vec3(this->worldTransformMatrix[1]));
    scale.z = glm::length(glm::vec3(this->worldTransformMatrix[2]));

    return scale;
}

glm::mat4 USceneComponent::GetWorldTransform() const
{
    return this->worldTransformMatrix;
}

void USceneComponent::RecalcWorldTransformHierarchy()
{
    if (m_parent.expired()) // root
    {
        // std::cout << "to update root worldTransform" << std::endl;
        // std::cout << "current localPosition: " << localPosition << std::endl;
        RecalcLocalTransform();
        this->worldTransformMatrix = this->localTransformMatrix;
    }
    else
    {
        RecalcLocalTransform();
        auto parentTransform       = m_parent.lock()->GetWorldTransform();
        this->worldTransformMatrix = parentTransform * (this->localTransformMatrix);
    }

    for (const auto& child : m_children)
    {
        // std::cout << "to update child worldTransform" << std::endl;
        child->RecalcWorldTransformHierarchy();
    }

    return;
}

void USceneComponent::AttachToParent(SharedPtr<USceneComponent>& parent)
{
    parent->m_children.push_back(shared_from_this());
    this->m_parent = parent;
}

SharedPtr<USceneComponent> USceneComponent::GetParent() const
{
    return m_parent.lock();
}

} // namespace Rudy
