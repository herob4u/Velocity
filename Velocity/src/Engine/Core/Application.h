#pragma once

#include "Window.h"
#include "LayerStack.h"

#include "Engine/Events/WindowEvents.h"

namespace Vct
{
    class Application
    {
    public:
        Application();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        // Main loop of application
        void Run();
    protected:

        void OnEvent(Event& e);

        bool OnWindowResized(WindowResizedEvent& e);
        bool OnWindowMoved(WindowMovedEvent& e);
        bool OnWindowMaximized(WindowMaximizedEvent& e);
        bool OnWindowClosed(WindowClosedEvent& e);
    private:
        std::unique_ptr<Window> m_Window;
        LayerStack m_LayerStack;
        bool bIsRunning;
    };
}
