#pragma once

#include "Event.h"

namespace Vct
{
    class WindowClosedEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(WindowClosed)

        WindowClosedEvent()
        {
        }
    };

    class WindowResizedEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(WindowResized)
        WindowResizedEvent(int width, int height)
            : Width(width)
            , Height(height)
        {
        }

        const int Width;
        const int Height;
    };

    class WindowMovedEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(WindowMoved)
        WindowMovedEvent(int x, int y)
            : X(x)
            , Y(y)
        {
        }

        const int X;
        const int Y;
    };

    class WindowFocusedEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(WindowFocused)
        WindowFocusedEvent()
        {
        }
    };

    class WindowMaximizedEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(WindowMaximized)
        WindowMaximizedEvent(bool maximized)
            : bMaximized(maximized)
        {
        }

        const bool bMaximized;
    };
}