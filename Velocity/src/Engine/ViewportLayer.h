#pragma once

#include "Core/Layer.h"
#include "Renderer/RenderCommands.h"
#include "Renderer/Camera.h"
#include "Renderer/ShaderProgram.h"

namespace Vct
{
    class ViewportLayer : public Layer
    {
    public:
        ViewportLayer();

        virtual void OnAttached() override;
        virtual void OnUpdate(float dt) override;
        virtual void OnDetached() override;
    };
}