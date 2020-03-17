#include "vctPCH.h"

#include "Application.h"
#include "Time.h"
#include "Core.h"
#include "Log.h"

using namespace Vct;

class ExampleLayer : public Layer
{
public:
    ExampleLayer()
        : Layer("ExampleLayer")
    {
    }

    virtual void OnAttached() override { VCT_TRACE("Attached {0}", GetDebugName()); }
};

Application::Application()
    : bIsRunning(true)
{
    m_Window = std::unique_ptr<Window>(new Window(1280, 720, "Velocity"));
    m_Window->SetEventCallback(BIND_FN(&Application::OnEvent));
    //m_Window->launch(0, nullptr, m_Window.get(), 1280, 720, "Velocity", 60);
}

void Application::Run()
{
    while(bIsRunning)
    {
        // Calculate delta time
        float dt = Time::Tick();

        // Update layers
        for(Layer* layer : m_LayerStack)
        {
            layer->OnUpdate(dt);
        }

        m_Window->OnUpdate();
    }
}

void Application::PushLayer(Layer* layer)
{
    if(layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttached();
    }
}

void Application::PushOverlay(Layer* overlay)
{
    if(overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttached();
    }
}

void Application::OnEvent(Event& e)
{
    //VCT_TRACE("{0}", e.ToString());
    EventDispatcher dispatcher(e);

    dispatcher.Dispatch<WindowClosedEvent>(BIND_FN(&Application::OnWindowClosed));

    for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
    {
        if(e.Handled)
            break;

        (*it)->OnEvent(e);
    }
}

bool Application::OnWindowResized(WindowResizedEvent& e)
{
    return true;
}

bool Application::OnWindowMoved(WindowMovedEvent& e)
{
    return true;
}

bool Application::OnWindowMaximized(WindowMaximizedEvent& e)
{
    return true;
}

bool Application::OnWindowClosed(WindowClosedEvent& e)
{
    bIsRunning = false;
    return true;
}
