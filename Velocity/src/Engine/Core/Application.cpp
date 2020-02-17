#include "Application.h"
#include "Log.h"

using namespace Vct;

Application::Application()
{
    Log::Init();

    m_Window = std::unique_ptr<Window>();

    //m_Window->launch(0, nullptr, m_Window.get(), 1280, 720, "Velocity", 60);
}

void Application::Run()
{
    while(bIsRunning)
    {
        // Calculate delta time

        // Update layers

        m_Window->OnUpdate();
    }
}
