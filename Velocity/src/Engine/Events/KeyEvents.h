#pragma once

#include "Event.h"

#include <sstream>

namespace Vct
{
    class KeyEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryInput | EventCategory::EventCategoryKeyboard)

        const int KeyCode;
    protected:
        KeyEvent(int keyCode)
            : KeyCode(keyCode)
        {
        }
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        EVENT_CLASS_TYPE(KeyPressed)

        KeyPressedEvent(int keyCode, int repeatCount)
            : KeyEvent(keyCode)
            , RepeatCount(repeatCount)
        {
        }

        const int RepeatCount;

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << GetName() << "(" << KeyCode << ")" << ", " << RepeatCount << " repeats\n";
            return ss.str();
        }
    };

    class KeyReleaseEvent : public KeyEvent
    {
    public:
        EVENT_CLASS_TYPE(KeyReleased)

        KeyReleaseEvent(int keyCode)
            : KeyEvent(keyCode)
        {
        }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << GetName() << "(" << KeyCode << ")\n";
            return ss.str();
        }
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        EVENT_CLASS_TYPE(KeyTyped)

        KeyTypedEvent(int keyCode)
            : KeyEvent(keyCode)
        {
        }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << GetName() << "(" << KeyCode << ")\n";
            return ss.str();
        }
    };
}