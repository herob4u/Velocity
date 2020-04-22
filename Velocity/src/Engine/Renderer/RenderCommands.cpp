#include "vctPCH.h"
#include "RenderCommands.h"

#include "glad/glad.h"

using namespace Vct;

void RenderCommands::ClearBuffers()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderCommands::SetClearColor(const glm::vec3 & color)
{
    glClearColor(color.r, color.g, color.b, 1.f);
}

void RenderCommands::SetClearColor(float r, float g, float b)
{
    glClearColor(r, g, b, 1.f);
}

void RenderCommands::SetWireframeMode(bool enabled)
{
    enabled ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

