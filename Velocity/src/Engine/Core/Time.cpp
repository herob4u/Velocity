#include "vctPCH.h"
#include "Time.h"

#include "GLFW/glfw3.h"

using namespace Vct;

static float s_LastFrameTime = 0;

float Time::GetTime()
{
    return (float)glfwGetTime();
}

float Time::GetTimeMilli()
{
    return (float) (glfwGetTime() * 1000.0f);
}

float Time::Tick()
{
    float time = GetTime();
    float dt = time - s_LastFrameTime;
    s_LastFrameTime = time;

    return dt;
}

float Time::GetLastFrameTime()
{
    return s_LastFrameTime;
}
