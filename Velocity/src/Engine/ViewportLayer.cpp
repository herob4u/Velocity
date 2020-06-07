#include "vctPCH.h"
#include "ViewportLayer.h"

/* Experimental */
#include "Engine/Renderer/Texture/Texture.h"
#include "Engine/Renderer/Texture/TextureMgr.h"
#include "Engine/IO/FileMgr.h"

using namespace Vct;

ViewportLayer::ViewportLayer()
    : Layer("ViewportLayer")
{
    FileMgr& fileMgr = FileMgr::Get();
    ResourceMgr* texMgr = ResourceMgrRegistry::Get().GetMgr(Texture::GetStaticType());
    m_BackgroundTexture = (Texture*)texMgr->GetResource(fileMgr.GetAbsPath("Pinup_A.tga"));

    ASSERT(m_BackgroundTexture.Get(), "Null texture!");

    if(!m_BackgroundTexture.IsValid())
        m_BackgroundTexture.Load();

    const std::string vertShader = ShaderProgram::getShaderFile("TexturedQuad.vs");
    const std::string fragShader = ShaderProgram::getShaderFile("TexturedQuad.fs");

    m_TexturedQuadShader = std::make_shared<ShaderProgram>();
    m_TexturedQuadShader->generateProgramObject();
    m_TexturedQuadShader->attachVertexShader(vertShader.c_str());
    m_TexturedQuadShader->attachFragmentShader(fragShader.c_str());
    m_TexturedQuadShader->link();
}

void ViewportLayer::OnAttached()
{
}

void ViewportLayer::OnUpdate(float dt)
{
    RenderCommands::ClearBuffers();
    RenderCommands::SetClearColor(0.2f, 0.2f, 0.2f);
    //VCT_TRACE("Asset Dir: {0}", ASSET_DIR);

    RenderCommands::DrawImage(m_TexturedQuadShader, m_BackgroundTexture.Get());
}

void ViewportLayer::OnDetached()
{
}
