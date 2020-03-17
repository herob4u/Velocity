#include "vctPCH.h"
#include "Window.h"

#include "Engine/Events/KeyEvents.h"
#include "Engine/Events/MouseEvents.h"
#include "Engine/Events/WindowEvents.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "Log.h"

using namespace Vct;

static bool s_GLContextInitialized = false;

static void GLFWErrorHandler(int errorCode, const char* description)
{
    VCT_ERROR("GL Error[{0}]: {1}", errorCode, description);
}

Window::Window(unsigned short width, unsigned short height, const std::string& title)
{
    m_WindowData.Width  = width;
    m_WindowData.Height = height;
    m_WindowData.Title  = title;
    m_WindowData.bVSync = false;

    Init();
}

Window::~Window()
{
    Shutdown();
}

void Window::OnUpdate()
{
    // Clear buffers
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}

void Window::SetEventCallback(EventCallbackFn callback)
{
    m_WindowData.EventCb = callback;
}

void Window::SetVSyncEnabled(bool val)
{
    val ? glfwSwapInterval(1) : glfwSwapInterval(0);
    m_WindowData.bVSync = val;
}    
     
void Window::Init()
{
    VCT_INFO("Initializing Window '{0}': {1}x{2}", m_WindowData.Title, m_WindowData.Width, m_WindowData.Height);

    if(!s_GLContextInitialized)
    {
        bool success = (glfwInit() == GLFW_TRUE);
        ASSERT(success);

        s_GLContextInitialized = true;
        glfwSetErrorCallback(GLFWErrorHandler);
    }
    
    // Can construct a window after GLFW is successfully initialized
    m_Window = glfwCreateWindow(m_WindowData.Width, m_WindowData.Height, m_WindowData.Title.c_str(), NULL, NULL);

    // Initialize the OpenGL context
    glfwMakeContextCurrent(m_Window);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    ASSERT(status, "Failed to Initialize Glad!");

    glfwSetWindowUserPointer(m_Window, &m_WindowData);

    // Initialize Callbacks
    BindCallbacks();
}    
     
void Window::Shutdown()
{
    if(m_Window)
    {
        VCT_WARN("Destroying Window");
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
}

void Window::BindCallbacks()
{
    // Mouse Movement
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
    {
        WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));

        MouseMovedEvent e(static_cast<float>(x), static_cast<float>(y));
        data.EventCb(e);
    });

    // Mouse Scroll
    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double scrollX, double scrollY)
    {
        WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));

        MouseScrolledEvent e( static_cast<float>(scrollX), static_cast<float>(scrollY));
        data.EventCb(e);
    });

    // Mouse Button
    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int modifiers)
    {
        WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));

        switch(action)
        {
            case GLFW_PRESS:
            {
                MouseButtonPressed e(button);
                data.EventCb(e);
            }
            break;

            case GLFW_RELEASE:
            {
                MouseButtonReleased e(button);
                data.EventCb(e);
            }
            break;
        }
    });

    // Key Press
    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scan, int action, int modifiers)
    {
        WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));

        switch(action)
        {
            case GLFW_PRESS:
            {
                KeyPressedEvent e(key, 0);
                data.EventCb(e);
            }
            break;

            case GLFW_REPEAT:
            {
                KeyPressedEvent e(key, 1);
                data.EventCb(e);
            }
            break;

            case GLFW_RELEASE:
            {
                KeyReleaseEvent e(key);
                data.EventCb(e);
            }
            break;
        }
    });

    // Character Typed
    glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int c)
    {
        WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));

        KeyTypedEvent e(c);
        data.EventCb(e);
    });

    // Window Close
    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
    {
        WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));

        WindowClosedEvent e;
        data.EventCb(e);
    });

    // Window Reposition (Upper Left corner)
    glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y)
    {
        WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));

        WindowMovedEvent e(x,y);
        data.EventCb(e);
    });

    // Window Resize
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
    {
        WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));

        WindowResizedEvent e(width, height);
        data.EventCb(e);
    });

    // Window Maximized
    glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
    {
        WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));

        WindowMaximizedEvent e( (maximized == GLFW_TRUE) );
        data.EventCb(e);
    });
}
