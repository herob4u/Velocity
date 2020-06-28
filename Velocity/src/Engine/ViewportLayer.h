#pragma once

#include "Core/Layer.h"
#include "Renderer/RenderCommands.h"
#include "Renderer/Camera.h"
#include "GameFramework/CameraController.h"
#include "Renderer/ShaderProgram.h"

#include "Engine/Resources/Resource.h"

namespace Vct
{
    class Shader;
    class LambertMaterial;
    class LambertMaterialInstance;
    class Model;

    class ViewportLayer : public Layer
    {
    public:
        ViewportLayer();

        virtual void OnAttached() override;
        virtual void OnUpdate(float dt) override;
        virtual void OnDetached() override;
        virtual void OnEvent(Event& e) override;
    private:
        TResourcePtr<class Texture2D> m_BackgroundTexture;
        std::shared_ptr<ShaderProgram> m_TexturedQuadShader;
        std::shared_ptr<Shader> m_PBRShader;

        std::unique_ptr<Model> m_CubeModel;
        TResourcePtr<Model> m_SphereModel;
        TResourcePtr<Model> m_FreyaModel;
        std::unique_ptr<LambertMaterial> m_LambertMaterial;
        std::unique_ptr<LambertMaterialInstance> m_CubeMaterial;
        class Framebuffer* m_ScreenshotBuffer;

        // Camera controls
        Camera m_Camera;
        FreeCameraController m_CameraController;
        float m_PrevX;
        float m_PrevY;
    };    
}