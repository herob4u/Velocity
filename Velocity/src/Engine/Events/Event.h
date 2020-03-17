#pragma once

#include "Engine/Core/Core.h"

namespace Vct
{
    enum class EventType
    {
        None = 0,
        WindowClosed, WindowResized, WindowFocused, WindowLostFocus, WindowMoved, WindowMaximized,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        //EventCategoryMouseButton = BIT(4)
    };

    #define EVENT_CLASS_TYPE(type) \
    static EventType GetStaticType() { return EventType::##type; } \
    virtual EventType GetEventType() const override { return GetStaticType(); }\
    virtual const char* GetName() const override { return #type; }

    #define EVENT_CLASS_CATEGORY(category)  virtual int  GetCategoryFlags() const override { return category; }

    class Event
    {
    public:
        virtual EventType               GetEventType() const = 0;
        virtual const char*             GetName() const = 0;
        virtual int                     GetCategoryFlags() const = 0;
        virtual std::string             ToString() const { return GetName(); }

        inline bool IsInCategory(EventCategory category)
        {
            return ((GetCategoryFlags() & category) != 0);
        }

        bool Handled = false;
    };

    /* Event Dispatcher to invoke functions with type safety*/
    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event)
            : m_Event(event)
        {
        }

        // F will be deduced by the compiler
        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if(m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled = func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }
    private:
        Event& m_Event;
    };
}