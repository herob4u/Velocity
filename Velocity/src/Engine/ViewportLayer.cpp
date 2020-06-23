#include "vctPCH.h"
#include "ViewportLayer.h"

/* Experimental */
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture/Image.h"
#include "Engine/Renderer/Texture/Texture2D.h"
#include "Engine/IO/FileMgr.h"

#include "Engine/Renderer/Mesh/Model.h"
#include "Engine/Core/Application.h"
#include "Engine/Engine.h"

using namespace Vct;

ViewportLayer::ViewportLayer()
    : Layer("ViewportLayer")
    , m_Camera(Camera(45.f, 16.f/9.f))
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

    Shader skyboxShader = Shader("Skybox");
    gEngine->GetShaderCache().SaveCache();
}

void ViewportLayer::OnAttached()
{

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
    
}

void ViewportLayer::OnUpdate(float dt)
{
    Renderer& renderer = Renderer::Get();

    RenderCommands::ClearBuffers();
    RenderCommands::SetClearColor(0.2f, 0.2f, 0.2f);
    //VCT_TRACE("Asset Dir: {0}", ASSET_DIR);

    renderer.BeginScene(m_Camera);
        RenderCommands::DrawImage(m_TexturedQuadShader, m_BackgroundTexture.Get());
    renderer.EndScene();

    Shader skyboxShader = Shader("Skybox");
}

void ViewportLayer::OnDetached()
{
}
