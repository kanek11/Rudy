#include "RudyPCH.h"

#include "LightComponent.h"

namespace Rudy
{

ULightComponent::ULightComponent() :
    USceneComponent() { }

USunlightComponent::USunlightComponent() :
    ULightComponent()
{
    SetDirection(m_direction);
}

void USunlightComponent::TickComponent(float DeltaTime)
{
    // call parent tick
    ULightComponent::TickComponent(DeltaTime);

    // update light space matrix
    m_viewMatrix       = glm::lookAt(this->GetWorldPosition(), glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f));
    m_projectionMatrix = glm::ortho(l, r, b, t, n, f);
}

void USunlightComponent::SetLightColor(const glm::vec3& color)
{
    m_lightColor = color;
}

glm::vec3 USunlightComponent::GetLightColor()
{
    return m_lightColor;
}

void USunlightComponent::SetDirection(const glm::vec3& direction)
{
    m_direction = glm::normalize(direction);
    SetWorldPosition(-m_direction * m_distance_to_origin);
}

glm::vec3 USunlightComponent::GetDirection()
{
    return m_direction;
}

void USunlightComponent::SetIntensity(float intensity)
{
    m_intensity = intensity;
}

float USunlightComponent::GetIntensity()
{
    return m_intensity;
}

void USunlightComponent::SetLitPassState(SharedPtr<Shader> _shader)
{
    // set light info
    _shader->SetVec3("u_DirLight.direction", m_direction);
    _shader->SetVec3("u_DirLight.color", m_lightColor);
    _shader->SetFloat("u_DirLight.intensity", m_intensity);

    auto _projView = this->GetLightSpaceMatrix();
    _shader->SetMat4("u_LightSpaceMatrix", _projView);
    //_shader->SetMat4("u_LightSpaceMatrix", LightSpaceCamera->GetProjectionViewMatrix());  //werid bug when create as "temp¡°£»
}

void USunlightComponent::SetShadowPassState(SharedPtr<Shader> _shader)
{
    _shader->SetMat4("u_view", m_viewMatrix);
    _shader->SetMat4("u_projection", m_projectionMatrix);
}

UPointLightComponent::UPointLightComponent() :
    ULightComponent()
{
    auto _pos = GetWorldPosition();

    m_projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    m_viewMatrices     = {
        glm::lookAt(_pos, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(_pos, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(_pos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(_pos, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(_pos, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(_pos, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };
}

void UPointLightComponent::TickComponent(float DeltaTime)
{
    // call parent tick
    ULightComponent::TickComponent(DeltaTime);

    auto _pos      = GetWorldPosition();
    m_viewMatrices = {
        glm::lookAt(_pos, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(_pos, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(_pos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(_pos, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(_pos, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(_pos, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };
}

} // namespace Rudy