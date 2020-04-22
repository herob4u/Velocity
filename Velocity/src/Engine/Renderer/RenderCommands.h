#pragma once

#include "glm/vec3.hpp"

namespace Vct
{
    class RenderCommands
    {
    public:
        static void ClearBuffers();
        static void SetClearColor(const glm::vec3& color);
        static void SetClearColor(float r, float g, float b);

        static void SetWireframeMode(bool enabled);
    };
}
