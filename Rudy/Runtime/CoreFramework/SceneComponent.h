#pragma once
#include "RudyPCH.h"

#include "Runtime/CoreFramework/ActorComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// glm quaternion :
// https://glm.g-truc.net/0.9.0/api/a00135.html

// there can be two equivalent representations the model transform,
// only expose local to user,  let vec/quat  drives the matrix
// world transform is recalculated by the system,  not exposed to user;
// query world vec/quat/scale always done by decomposing the world matrix
// if it's not accessed,  it doesnt even need to be explicit state;

namespace Rudy
{

class USceneComponent : public UActorComponent, public std::enable_shared_from_this<USceneComponent>
{
public:
    virtual ~USceneComponent() = default;
    USceneComponent();

    void TickComponent(float DeltaTime) override;

public:
    void AttachToParent(SharedPtr<USceneComponent>& parent);

    void RecalcWorldTransformHierarchy();

    // get parent
    SharedPtr<USceneComponent> GetParent() const;
    uint32_t                   GetChildrenCount() const;

    void SetLocalPosition(const glm::vec3& position);
    void SetLocalRotation(const glm::quat& rotation);
    void SetLocalScale(const glm::vec3& scale);

    glm::mat4 GetWorldTransform() const;
    glm::vec3 GetWorldPosition() const;
    void      SetWorldPosition(const glm::vec3& position);
    glm::quat GetWorldRotation() const;
    void      SetWorldRotation(const glm::quat& rotation);
    glm::vec3 GetWorldScale() const;
    void      SetWorldScale(const glm::vec3& scale);

private:
    void RecalcLocalTransform();
    void RecalcWorldTransform();

private:
    glm::vec3 localPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // wxyz, unit quaternion
    glm::vec3 localScale    = glm::vec3(1.0f, 1.0f, 1.0f);

    // system implementation
    // transform as a whole affine matrix
    glm::mat4 localTransformMatrix = glm::mat4(1.0f);
    glm::mat4 worldTransformMatrix = glm::mat4(1.0f);

    // the hierarchy
    WeakPtr<USceneComponent>                m_parent;
    std::vector<SharedPtr<USceneComponent>> m_children;

    bool m_isLocalDirty = true;
    // if the local transform is changed,  the world transform is dirty;
};

} // namespace Rudy