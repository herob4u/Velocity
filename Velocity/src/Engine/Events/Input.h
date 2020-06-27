#pragma once

#include "Constants.h"

namespace Vct
{
    class Input
    {
    public:
        static bool IsKeyPressed(int key);
        static bool IsMousePressed(int button);
        static float GetCursorX();
        static float GetCursorY();
    };
}