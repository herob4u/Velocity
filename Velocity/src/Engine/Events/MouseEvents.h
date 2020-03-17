#pragma once

#include "Event.h"
#include <sstream>

namespace Vct
{
    class MouseMovedEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
        EVENT_CLASS_TYPE(MouseMoved)
        MouseMovedEvent(float posX, float posY)
            : X(posX)
            , Y(posY)
        {
        }

        const float X;
        const float Y;

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << GetName() << "(" << X << ", " << Y << ")";
            return ss.str();
        }
    };

    class MouseScrolledEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
        EVENT_CLASS_TYPE(MouseScrolled)
        MouseScrolledEvent(float dx, float dy)
            : ScrollX(dx)
            , ScrollY(dy)
        {
        }

        const float ScrollX;
        const float ScrollY;

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << GetName() << "(" << ScrollX << ", " << ScrollY << ")";
            return ss.str();
        }
    };

    class MouseButtonEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
        MouseButtonEvent(int button)
            : Button(button)
        {
        }

        const int Button;
    };

    class MouseButtonPressed : public MouseButtonEvent
    {
    public:
        EVENT_CLASS_TYPE(MouseButtonPressed)
        MouseButtonPressed(int button)
            : MouseButtonEvent(button)
        {
        }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << GetName() << ": " << Button;
            return ss.str();
        }
    };

    class MouseButtonReleased : public MouseButtonEvent
    {
    public:
        EVENT_CLASS_TYPE(MouseButtonReleased)
        MouseButtonReleased(int button)
        : MouseButtonEvent(button)
        {
        }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << GetName() << ": " << Button;
            return ss.str();
        }
    };
}