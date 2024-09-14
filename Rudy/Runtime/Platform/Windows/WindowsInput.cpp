

#include "RudyPCH.h"

#include "Runtime/Events/InputManager.h"

#include <GLFW/glfw3.h>

namespace Rudy
{

void* InputManager::s_WindowContext;

bool InputManager::IsKeyPressed(const KeyCode key)
{
    // auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto* window = static_cast<GLFWwindow*>(s_WindowContext);
    auto  state  = glfwGetKey(window, static_cast<int32_t>(key));
    return state == GLFW_PRESS;
}

bool InputManager::IsMouseButtonPressed(const MouseButtonCode button)
{
    auto* window = static_cast<GLFWwindow*>(s_WindowContext);
    auto  state  = glfwGetMouseButton(window, static_cast<int32_t>(button));
    return state == GLFW_PRESS;
}

glm::vec2 InputManager::GetMousePosition()
{
    auto*  window = static_cast<GLFWwindow*>(s_WindowContext);
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return { (float)xpos, (float)ypos };
}

float InputManager::GetMouseX()
{
    return GetMousePosition().x;
}

float InputManager::GetMouseY()
{
    return GetMousePosition().y;
}

} // namespace Rudy