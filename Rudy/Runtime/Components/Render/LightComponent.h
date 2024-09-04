#pragma once
#include "RudyPCH.h"

#include "CameraComponent.h"
#include "Runtime/CoreFramework/SceneComponent.h"
#include "Runtime/Renderer/Shader.h"

// todo: dirty flag for light components

// glm::vec3 position = glm::vec3(0.0f);
// glm::vec3 direction = glm::vec3(0.0f);
// glm::vec3 color = glm::vec3(1.0f);
// float intensity = 1.0f;
// float constant = 1.0f;
// float linear = 0.09f;
// float quadratic = 0.032f;
// float cutOff = glm::cos(glm::radians(12.5f));
// float outerCutOff = glm::cos(glm::radians(15.0f));

// enum class LightType
//{
//    None = 0,
//    POINT,
//    DIRECTIONAL,
//    SPOTLIGHT,
//    // AREA
//};

namespace Rudy
{

//<<abstract>>
class ULightComponent : public USceneComponent
{
public:
    virtual ~ULightComponent() = default;
    ULightComponent();
};

//================================================================================================
//<<terminal>>
class USunlightComponent : public ULightComponent
{
public:
    ~USunlightComponent() = default;
    USunlightComponent();

public:
    void TickComponent(float DeltaTime) override;

public:
    void SetShadowPassState(SharedPtr<Shader> _shader);
    void SetLitPassState(SharedPtr<Shader> _shader);

public:
    void      SetLightColor(const glm::vec3& color);
    glm::vec3 GetLightColor();
    void      SetIntensity(float intensity);
    float     GetIntensity();
    void      SetDirection(const glm::vec3& direction);
    glm::vec3 GetDirection();

    glm::mat4 GetViewMatrix() const { return m_viewMatrix; }
    glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
    glm::mat4 GetLightSpaceMatrix() const { return m_projectionMatrix * m_viewMatrix; }

private:
    glm::vec3 m_lightColor = glm::vec3(1.0f);
    float     m_intensity  = 1.0f;
    glm::vec3 m_direction  = glm::vec3(1.0f, -0.5f, -1.0f);

public:
    // SharedPtr<UCameraComponent> LightSpaceCamera = nullptr;

    glm::mat4 m_viewMatrix       = glm::mat4(1.0f);
    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

    // the frustum size
    float m_distance_to_origin = 5.0f;
    float l = -10.0f, r = 10.0f, b = -10.0f, t = 10.0f, n = -50.0f, f = 100.0f;
};

class UPointLightComponent : public ULightComponent
{
public:
    ~UPointLightComponent() = default;
    UPointLightComponent();

public:
    void TickComponent(float DeltaTime) override;

private:
    glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float     m_intensity  = 1.0f;

    // more control
    // float m_radius = 1.0f;
    // float m_falloff = 1.0f;
    // float m_attenuation = 1.0f;

private:
    std::array<glm::mat4, 6> m_viewMatrices;
    glm::mat4                m_projectionMatrix = glm::mat4(1.0f);
};

} // namespace Rudy