#pragma once

#include "Window.h"

namespace Vct
{
    class Application
    {
    public:
        Application();

    private:
        // Main loop of application
        void Run();

    private:
        std::unique_ptr<Window> m_Window;

        bool bIsRunning;
    };
}
