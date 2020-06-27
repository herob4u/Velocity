#include "vctPCH.h"
#include "ViewportLayer.h"

/* Experimental */
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture/Image.h"
#include "Engine/Renderer/Texture/Texture2D.h"
#include "Engine/IO/FileMgr.h"

#include "Engine/Renderer/Mesh/Model.h"
#include "Engine/Renderer/Mesh/Mesh.h"
#include "Engine/Renderer/Mesh/MeshFactory.h"

#include "Engine/Renderer/Materials/LambertMaterial.h"

#include "Engine/Core/Application.h"
#include "Engine/Engine.h"

#include "Engine/Events/KeyEvents.h"
#include "Engine/Events/MouseEvents.h"
#include "Engine/Events/Input.h"

#include "glm/gtc/matrix_transform.hpp"

using namespace Vct;

ViewportLayer::ViewportLayer()
    : Layer("ViewportLayer")
    , m_Camera(Camera(45.f, 16.f/9.f))
    , m_CameraController(m_Camera)
    , m_PrevX(0.f)
    , m_PrevY(0.f)
{
    const Window& window = Application::Get().GetWindow();

    FramebufferParams fbParams;
    fbParams.Width  = window.GetWidth();
    fbParams.Height = window.GetHeight();

    m_ScreenshotBuffer = Framebuffer::CreateColorBuffer(0, fbParams);

    FileMgr& fileMgr = FileMgr::Get();
    ResourceMgr* texMgr = ResourceMgrRegistry::Get().GetMgr(Texture2D::GetStaticType());
    m_BackgroundTexture = texMgr->GetResource<Texture2D>("beardipped.bmp");

    ASSERT(m_BackgroundTexture.Get(), "Null texture!");

    if(!m_BackgroundTexture.IsValid())
    {
        //m_BackgroundTexture.Load();
        std::vector<Path> paths = { m_BackgroundTexture.ResPath };
        texMgr->LoadAsync(paths, nullptr);
    }

    const std::string vertShader = ShaderProgram::getShaderFile("TexturedQuad.vs");
    const std::string fragShader = ShaderProgram::getShaderFile("TexturedQuad.fs");

    m_TexturedQuadShader = std::make_shared<ShaderProgram>();
    m_TexturedQuadShader->generateProgramObject();
    m_TexturedQuadShader->attachVertexShader(vertShader.c_str());
    m_TexturedQuadShader->attachFragmentShader(fragShader.c_str());
    m_TexturedQuadShader->link();

    m_LambertMaterial = std::make_unique<LambertMaterial>();
    m_CubeMaterial = std::make_unique<LambertMaterialInstance>(m_LambertMaterial.get());

    m_CubeModel.reset(Model::FromMesh(MeshFactory::MakeBox(1, 1, 1), m_CubeMaterial.get()));

    gEngine->GetShaderCache().SaveCache();
}

void ViewportLayer::OnAttached()
{
#ifdef SCREENSHOT_TEST
    Renderer& renderer = Renderer::Get();

    RenderCommands::ClearBuffers();
    RenderCommands::SetClearColor(0.2f, 0.2f, 0.2f);

    //while(!m_BackgroundTexture.IsValid()) {}

    renderer.BeginScene(m_Camera);
    Renderer::Bind(*m_ScreenshotBuffer);
    RenderCommands::DrawImage(m_TexturedQuadShader, m_BackgroundTexture.Get());
    Renderer::Unbind(*m_ScreenshotBuffer);
    renderer.EndScene();

    Texture2D* screenshot = (Texture2D*)m_ScreenshotBuffer->GetTextureTarget();
    Ref<Image> screenshot_img = screenshot->RenderToImage();
    screenshot_img->Write(std::string(ASSET_DIR) + "screenshot.tga");
#endif
}

void ViewportLayer::OnUpdate(float dt)
{
    Renderer& renderer = Renderer::Get();

    RenderCommands::ClearBuffers();
    RenderCommands::SetClearColor(0.2f, 0.2f, 0.2f);
    //VCT_TRACE("Asset Dir: {0}", ASSET_DIR);

    renderer.BeginScene(m_Camera);
        //RenderCommands::DrawImage(m_TexturedQuadShader, m_BackgroundTexture.Get());
        renderer.Submit(m_CubeModel->GetMesh(0), glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -5)), m_CubeModel->GetMaterial(0));
    renderer.EndScene();

    m_CameraController.Update(dt);
    //Shader skyboxShader = Shader("Skybox");
}

void ViewportLayer::OnDetached()
{
}

void ViewportLayer::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);

    dispatcher.Dispatch<KeyPressedEvent>([=](KeyPressedEvent& e) 
    {
        bool handled = false;
        if(e.KeyCode == KeyCode::W)
        {
            m_CameraController.OnBeginMoveZ(1.f);
            handled = true;
        }
        else if(e.KeyCode == KeyCode::S)
        {
            m_CameraController.OnBeginMoveZ(-1.f);
            handled = true;
        }

        if(e.KeyCode == KeyCode::D)
        {
            m_CameraController.OnBeginMoveX(1.f);
            handled = true;
        }
        else if(e.KeyCode == KeyCode::A)
        {
            m_CameraController.OnBeginMoveX(-1.f);
            handled = true;
        }
        
        return handled;
    });

    dispatcher.Dispatch<KeyReleaseEvent>([=](KeyReleaseEvent& e)
    {
        bool handled = false;
        if(e.KeyCode == KeyCode::W)
        {
            m_CameraController.OnEndMoveZ(1.f);
            handled = true;
        }
        else if(e.KeyCode == KeyCode::S)
        {
            m_CameraController.OnEndMoveZ(-1.f);
            handled = true;
        }

        if(e.KeyCode == KeyCode::D)
        {
            m_CameraController.OnEndMoveX(1.f);
            handled = true;
        }
        else if(e.KeyCode == KeyCode::A)
        {
            m_CameraController.OnEndMoveX(-1.f);
            handled = true;
        }

        return handled;
    });

    dispatcher.Dispatch<MouseButtonPressed>([=](MouseButtonPressed& e)
    {
        if(e.Button == MouseButton::RightMouse)
        {
            m_PrevX = Input::GetCursorX();
            m_PrevY = Input::GetCursorY();

            return true;
        }
        return false;
    });

    dispatcher.Dispatch<MouseMovedEvent>([=](MouseMovedEvent& e)
    {
        if(Input::IsMousePressed(MouseButton::RightMouse))
        {
            m_CameraController.OnMouseInputX(e.X - m_PrevX);
            m_CameraController.OnMouseInputY(e.Y - m_PrevY);

            m_PrevX = e.X;
            m_PrevY = e.Y;

            return true;
        }

        return false;
    });
}
