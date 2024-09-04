#pragma once

#include <glm/glm.hpp>

#include "Runtime/Events/Input.h"
#include "Runtime/CoreFramework/SceneComponent.h"
#include "Runtime/Renderer/Shader.h"

namespace Rudy
{

class UCameraComponent : public USceneComponent
{
public:
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };

public:
    ~UCameraComponent() = default;
    UCameraComponent();

    void TickComponent(float DeltaTime) override;

    void SetRenderState(SharedPtr<Shader> _shader);

    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix() const;
    const glm::mat4  GetProjectionViewMatrix() const;

    glm::vec3 GetForwardVector();
    glm::vec3 GetUpVector();

    void RecalcViewMatrix();
    void RecalcProjectionMatrix();

    void SetFocusAndWorldRot(const glm::vec3& focalPoint);
    void SetForwardAndWorldRot(const glm::vec3& forward);

private:
    void HandleUserInput();
    void MouseRotate(const glm::vec2& delta);
    void MouseZoom(const glm::vec2& delta);
    void MousePan(const glm::vec2& delta);

public:
    // perspective
    float FOV         = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f, farPlane = 1000.0f;

    // orthographic
    float left = -5.0f, right = 5.0f, top = 5.0f, bottom = -5.0f;

    ProjectionType projectionType = ProjectionType::Perspective;

private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    // navigation stuff
private:
    glm::vec3 m_focalPoint   = { 0.0f, 0.0f, 0.0f }; // look at;
    glm::vec2 m_lastMousePos = { 0.0f, 0.0f };

    bool  m_AltPressedLastFrame = false;
    float m_sensitivity         = 0.01f; // scale the mouse movement. on screen space.

    float m_Distance = 0; //= length(position - focalpoint)
};

} // namespace Rudy