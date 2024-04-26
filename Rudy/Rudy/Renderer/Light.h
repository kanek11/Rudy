#pragma once
#include "RudyPCH.h"
#include "Object.h"

// the light objects are mostly structural data;
//
// LightType type = LightType::None;
// glm::vec3 position = glm::vec3(0.0f);
// glm::vec3 direction = glm::vec3(0.0f);
// glm::vec3 color = glm::vec3(1.0f);
// float intensity = 1.0f;
// float constant = 1.0f;
// float linear = 0.09f;
// float quadratic = 0.032f;
// float cutOff = glm::cos(glm::radians(12.5f));
// float outerCutOff = glm::cos(glm::radians(15.0f));

namespace Rudy
{

// me: relating high level enums;
enum class LightType
{
    None = 0,
    POINT,
    DIRECTIONAL,
    SPOTLIGHT,
    // AREA, to do
};

//<<interface>>
class Light : public Object
{
public:
    ~Light() = default;
    Light();

    // todo: type, cast_shadow ,etc
};

//================================================================================================
class DirectionalLight : public Light
{
public:
    ~DirectionalLight() = default;
    DirectionalLight();

    static Ref<DirectionalLight> Create();

    //
    // void OnUpdate();

    //
    void SetColor(const glm::vec3& color);
    void SetIntensity(float intensity);
    void SetDirection(const glm::vec3& direction);

public:
    glm::vec3 m_color     = glm::vec3(1.0f);
    float     m_intensity = 1.0f;
    glm::vec3 m_direction = glm::vec3(1.0f, -0.5f, -1.0f);

    // shadow map info
    float near_plane = 1.0f;
    float far_plane  = 10.0f;

    Ref<Camera> lightSpaceCamera = nullptr;

    // int m_index = 0;
};

class PointLight : public Light
{
public:
    glm::vec3 Color     = glm::vec3(1.0f, 1.0f, 1.0f);
    float     intensity = 1.0f;

    // more control
    float radius = 1.0f;
};

} // namespace Rudy