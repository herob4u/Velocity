#pragma once

#include "Core/Layer.h"
#include "Renderer/RenderCommands.h"
#include "Renderer/Camera.h"
#include "Renderer/ShaderProgram.h"

#include "Engine/Resources/Resource.h"

namespace Vct
{
    class Shader;
    class ViewportLayer : public Layer
    {
    public:
        ViewportLayer();

        virtual void OnAttached() override;
        virtual void OnUpdate(float dt) override;
        virtual void OnDetached() override;

    private:
        TResourcePtr<class Texture2D> m_BackgroundTexture;
        std::shared_ptr<ShaderProgram> m_TexturedQuadShader;
        std::shared_ptr<Shader> m_FlatShader;
        std::shared_ptr<Shader> m_PBRShader;
        class Framebuffer* m_ScreenshotBuffer;
        Camera m_Camera;
    };    
}