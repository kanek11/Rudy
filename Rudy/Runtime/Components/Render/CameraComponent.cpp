#include "RudyPCH.h"

#include "CameraComponent.h"

namespace Rudy
{

UCameraComponent::UCameraComponent() :
    USceneComponent()
{
    SetLocalPosition(glm::vec3(0.0f, 2.0f, 5.0f));
    SetLocalRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    RecalcWorldTransformHierarchy();

    SetFocusAndWorldRot(glm::vec3(0.0f));

    RecalcWorldTransformHierarchy();

    RecalcViewMatrix();
    RecalcProjectionMatrix();
}

void UCameraComponent::RecalcProjectionMatrix()
{
    if (projectionType == ProjectionType::Perspective)
    {
        m_projectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
    }
    else if (projectionType == ProjectionType::Orthographic)
    {
        m_projectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    }
}

void UCameraComponent::RecalcViewMatrix()
{
    auto _pos = GetWorldPosition();
    // std::cout << _pos << std::endl;
    // std::cout << GetForwardVector() << std::endl;
    m_viewMatrix = glm::lookAt(_pos, _pos + GetForwardVector(), GetUpVector());
}

// quaternion * axis gives the orientation axis of the object;
// look at -z ;
glm::vec3 UCameraComponent::GetForwardVector()
{
    auto _quat = GetWorldRotation();
    return glm::normalize(glm::vec3(_quat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
}

glm::vec3 UCameraComponent::GetUpVector()
{
    auto _quat = GetWorldRotation();
    return glm::normalize(glm::vec3(_quat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
}

const glm::mat4& UCameraComponent::GetViewMatrix() const
{
    return m_viewMatrix;
}

const glm::mat4& UCameraComponent::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

const glm::mat4 UCameraComponent::GetProjectionViewMatrix() const
{
    auto _projView = m_projectionMatrix * m_viewMatrix; // do not return temporary;
    return _projView;
}

void UCameraComponent::TickComponent(float DeltaTime)
{
    USceneComponent::TickComponent(DeltaTime);

    HandleUserInput();

    RecalcViewMatrix();
    RecalcProjectionMatrix();
}

void UCameraComponent::SetRenderState(SharedPtr<Shader> _shader)
{
    _shader->SetMat4("u_view", GetViewMatrix());
    _shader->SetMat4("u_projection", GetProjectionMatrix());
}

void UCameraComponent::HandleUserInput()
{
    glm::vec2 _currMousePos = Input::GetMousePosition();

    if (!Input::IsKeyPressed(Key::LeftAlt))
    {
        m_AltPressedLastFrame = false;
        return;
    }
    else
    {
        if (!m_AltPressedLastFrame)
        {
            RD_CORE_INFO("Alt pressed");
            m_AltPressedLastFrame = true; // set for next frame, donothing in this frame
            return;
        }

        // update necessary info
        glm::vec2 delta = (_currMousePos - m_lastMousePos) * m_sensitivity;
        // m_Distance      = glm::length(m_Position - m_FocalPoint);

        // RD_CORE_INFO("camera: the delta is {0}, {1}", delta.x, delta.y);

        if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
            MouseRotate(delta);
        else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
            MouseZoom(delta);
        else if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
            MousePan(delta);
    }

    // finally update the last mouse position  after all the calculations
    m_lastMousePos = _currMousePos;
}

//
void UCameraComponent::MouseRotate(const glm::vec2& delta)
{
    glm::vec3 _forward = GetForwardVector();
    glm::vec3 _up      = GetUpVector();
    glm::vec3 _right   = glm::cross(-_forward, _up);

    glm::quat _rot = glm::angleAxis(0.2f * delta.x, _up) * glm::angleAxis(0.2f * delta.y, _right);

    SetWorldRotation(_rot * GetWorldRotation());
}

void UCameraComponent::MouseZoom(const glm::vec2& delta)
{
    glm::vec3 _forward = GetForwardVector();
    SetWorldPosition(GetWorldPosition() + _forward * delta.y);
}

void UCameraComponent::MousePan(const glm::vec2& delta)
{
    glm::vec3 _forward = GetForwardVector();
    glm::vec3 _up      = GetUpVector();
    glm::vec3 _right   = glm::cross(-_forward, _up);

    SetWorldPosition(GetWorldPosition() + _right * delta.x + _up * delta.y);
}

void UCameraComponent::SetFocusAndWorldRot(const glm::vec3& target)
{
    glm::vec3 _forward = glm::normalize(target - GetWorldPosition());
    SetForwardAndWorldRot(_forward);
}

// forward= -z;
void UCameraComponent::SetForwardAndWorldRot(const glm::vec3& forward)
{
    // std::cout << "setfocus forward: " << forward << std::endl;
    auto _newZ  = glm::normalize(-forward);
    auto _quat  = GetWorldRotation();
    auto _up    = glm::normalize(glm::vec3(_quat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
    auto _right = glm::normalize(glm::cross(_up, _newZ));
    auto _newUp = glm::normalize(glm::cross(_newZ, _right));

    auto _newRot = glm::quat_cast(glm::mat3(_right, _newUp, _newZ));

    // std::cout << "update forward: " << _newRot * glm::vec3(0.0f, 0.0f, -1.0f) << std::endl;
    SetWorldRotation(_newRot);
}

} // namespace Rudy