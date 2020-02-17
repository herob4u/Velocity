#pragma once

#include "cs488-framework/CS488Window.hpp"

namespace Vct
{
    class Window : public CS488Window
    {
    public:
        void OnUpdate();

        inline int GetWidth() const { return m_windowWidth; }
        inline int GetHeight() const { return m_windowHeight; }
    private:

    };
}
