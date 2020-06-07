#pragma once

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

class Texture;
class ShaderProgram;

namespace Vct
{
    class RenderCommands
    {
    public:
        static void ClearBuffers();
        static void SetClearColor(const glm::vec3& color);
        static void SetClearColor(float r, float g, float b);

        static void DrawImage(Ref<ShaderProgram> shader, Texture* texture);
        static void SetWireframeMode(bool enabled);
    };
}
