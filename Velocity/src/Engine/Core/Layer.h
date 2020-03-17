#pragma once
#include "Core.h"
#include "Engine/Events/Event.h"

#include <string>

namespace Vct
{
    class Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void OnEvent(Event& e) {}
        virtual void OnAttached() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnDetached() {}

        inline const std::string& GetDebugName() const { return m_DebugName; }
    private:
        const std::string m_DebugName;
    };
}