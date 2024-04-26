#include "RudyPCH.h"

#include "Light.h"

namespace Rudy
{

Light::Light() :
    Object() { isRenderable = false; }

DirectionalLight::DirectionalLight() :
    m_color(glm::vec3(1.0f, 1.0f, 1.0f)), m_direction(glm::vec3(0.0f, -1.0f, 0.0f)), m_intensity(1.0f)
{
}

Ref<DirectionalLight> DirectionalLight::Create()
{
    Ref<DirectionalLight> light = CreateRef<DirectionalLight>();
    // component
    light->InitComponent(light);
    return light;
}

void DirectionalLight::SetColor(const glm::vec3& color)
{
    m_color = color;
}

void DirectionalLight::SetDirection(const glm::vec3& direction)
{
    m_direction = direction;
}

void DirectionalLight::SetIntensity(float intensity)
{
    m_intensity = intensity;
}

} // namespace Rudy