#include "vctPCH.h"
#include "Input.h"

#include "Engine/Core/Application.h"
#include "Engine/Core/Window.h"

#include "GLFW/glfw3.h"

using namespace Vct;

bool Input::IsKeyPressed(int key)
{
    const int state = glfwGetKey((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), key);
    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

bool Input::IsMousePressed(int button)
{
    const int state = glfwGetMouseButton((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), button);
    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

float Input::GetCursorX()
{
    double x;
    glfwGetCursorPos((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), &x, nullptr);
    return static_cast<float>(x);
}

float Input::GetCursorY()
{
    double y;
    glfwGetCursorPos((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), nullptr, &y);
    return static_cast<float>(y);
}
