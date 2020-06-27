#pragma once

#include "Engine/Events/Event.h"

struct GLFWwindow;

namespace Vct
{
    class Window
    {
    public:
        // Represents the callback function the window calls to deliver events to. This is typically a job for the Application
        // The Window receives GLFW events and translates them to our Event types to be handled by the application. The Application
        // then chooses to handle the event or dispatch it to other layers.
        using EventCallbackFn = std::function<void(Event&)>;

        Window(unsigned short width, unsigned short height, const std::string& title);
        virtual ~Window();

        void OnUpdate();

        void SetEventCallback(EventCallbackFn callback);
        void SetVSyncEnabled(bool val);

        inline unsigned short GetWidth() const { return m_WindowData.Width; }
        inline unsigned short GetHeight() const { return m_WindowData.Height; }
        inline bool IsVSyncEnabled() const { return m_WindowData.bVSync; }
        inline void* GetNativeWindow() const { return m_Window; }
    private:
        void Init();
        void Shutdown();
        void BindCallbacks();
    private:
        struct WindowData
        {
            unsigned short Width;
            unsigned short Height;
            std::string Title;
            bool bVSync;
            EventCallbackFn EventCb;
        };
        
        WindowData m_WindowData;
        GLFWwindow* m_Window;
    };
}
