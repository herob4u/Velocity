#include "vctPCH.h"

#include "Renderer.h"
#include "ShaderProgram.h"
#include "Material.h"

#include "Texture/Texture2D.h"
#include "Texture/Cubemap.h"
#include "Mesh/Model.h"

#include "Engine/Core/Application.h"

#include "Engine/Resources/ResourceMgr.h"

#include "Engine/Engine.h"

#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"

using namespace Vct;

template <typename ResType>
class RenderResourceMgr : public ResourceMgr
{
    virtual Resource* CreateResource(const Path& resPath) override
    {
        return new ResType(resPath);
    }

    virtual void DestroyResource(Resource& res) override
    {
        ASSERT(!res.IsLoading(), "Cannot destroy while loading resource");

        // Placeholder code - this will spike the performance, but it should never occur to begin with
        while(res.IsLoading()) {}

        if(res.IsLoaded())
        {
            Unload(res.GetPath());
        }

        ASSERT(res.GetRefCount() == 0, "Resource still in use!");
        auto found = m_ResourceList.find(res.GetPath().GetPathId());
        if(found != m_ResourceList.end())
        {
            Resource* res = found->second;
            if(res)
            {
                delete res;
                res = nullptr;
            }

            m_ResourceList.erase(found);
        }
    }
};

Renderer::Renderer()
    : m_Finished(false)
{
}

Renderer::~Renderer()
{
    //m_Finished = true;
    //m_WorkerThread.join();
}

void Renderer::Init()
{
    RegisterMgrs();

    FramebufferParams fbParams;
    fbParams.Width = 512;
    fbParams.Height = 512;
    fbParams.TextureTarget.bIsCubemap = true;

    m_CubemapBuffer.reset(Framebuffer::CreateColorBuffer(0, fbParams));
}

void Renderer::Shutdown()
{
}

void Renderer::BeginScene(const Camera& camera)
{
    m_TextureCmdQueue.Flush();
    m_BufferCmdQueue.Flush();
}

void Renderer::EndScene()
{
}

void Renderer::RenderCubemap()
{
    // Get cubemap shader from shader mgr
    ResourceMgrRegistry& registry = ResourceMgrRegistry::Get();
    const ShaderProgram* shader = registry.GetResource<ShaderProgram>("Shaders/Skybox.vs");

    // static array of camera directions
    static const glm::vec3 UP(0.f, 1.f, 0.f);
    static const glm::vec3 RIGHT(1.f, 0.f, 0.f);
    static const glm::vec3 FORWARD(0.f, 0.f, 1.f);
    static const glm::vec3 ZERO(0.f);
    static glm::mat4 views[6] =
    {
        glm::lookAt(ZERO, RIGHT, -UP),
        glm::lookAt(ZERO, -RIGHT, -UP),
        glm::lookAt(ZERO, UP, FORWARD),
        glm::lookAt(ZERO, -UP, -FORWARD),
        glm::lookAt(ZERO, FORWARD, -UP),
        glm::lookAt(ZERO, -FORWARD, -UP)
    };

    // Loop and upload camera uniforms to shader, and render 1x1 cube
    Bind(*m_CubemapBuffer.get());
    for(int i = 0; i < 6; i++)
    {
        // Set view uniform here ...

        m_CubemapBuffer->SetCubemapTargetFace(static_cast<CubemapFace>(i));

        // Render here...
    }

    Unbind(*m_CubemapBuffer.get());
}

void Renderer::GenerateTextureAsync(uint32_t& texId, const uint8_t* data, uint16_t width, uint16_t height, uint16_t format, uint16_t type, uint16_t wrapMode)
{
   RenderCmd* cmd = new GenTexturesCmd(texId, data, width, height, format, type, wrapMode);
   m_TextureCmdQueue.Enqueue(cmd);
}

void Renderer::DeleteTextureAsync(uint32_t& texId)
{
    RenderCmd* cmd = new DelTexturesCmd(texId);
    m_TextureCmdQueue.Enqueue(cmd);
}

void Renderer::GenerateTexture(uint32_t& texId, const uint8_t* data, uint16_t width, uint16_t height, uint16_t format, uint16_t type, uint16_t wrapMode)
{
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void Renderer::DeleteTexture(Texture2D& texture)
{
    glDeleteTextures(1, &texture.RendererId);
}

void Renderer::GenerateFramebuffer(Framebuffer& fb)
{
    ASSERT(fb.m_RendererId == 0, "Framebuffer already occupied");
    glGenFramebuffers(1, &fb.m_RendererId);
    glBindFramebuffer(GL_FRAMEBUFFER, fb.m_RendererId);

    // Attaches the texture target to this framebuffer
    const uint8_t attachmentSlot = fb.GetAttachmentSlot();
    int glAttachment = 0;

    switch(fb.m_Type)
    {
        case Framebuffer::Type::COLOR: glAttachment = GL_COLOR_ATTACHMENT0 + attachmentSlot;
        break;

        case Framebuffer::Type::DEPTH: glAttachment = GL_DEPTH_ATTACHMENT;
        break;

        case Framebuffer::Type::STENCIL: glAttachment = GL_STENCIL_ATTACHMENT;
        break;

        case Framebuffer::Type::DEPTH_STENCIL: glAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
        break;
    }

    ASSERT(glAttachment != 0, "Invalid Framebuffer attachment");

    if(fb.m_Params.TextureTarget.bIsCubemap)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, glAttachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X, fb.m_Texture->GetRendererId(), 0);
    }
    else
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, glAttachment, GL_TEXTURE_2D, fb.m_Texture->GetRendererId(), 0);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        VCT_ERROR("Framebuffer incomplete: {0}", status);
        ASSERT(false, "Framebuffer incomplete", (int)glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DeleteFramebuffer(Framebuffer& fb)
{
    if(fb.m_RendererId != 0)
        glDeleteFramebuffers(1, &fb.m_RendererId);
}

void Renderer::Bind(Framebuffer& fb)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fb.GetRendererId());
}

void Renderer::Unbind(Framebuffer&)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer& Renderer::Get()
{
/*
    static Renderer self;
    return self;
*/
    return gEngine->GetRenderer();
}

void Renderer::RegisterMgrs()
{
    ResourceMgrRegistry& registry = ResourceMgrRegistry::Get();

    registry.Register(ShaderProgram::GetStaticType(), new RenderResourceMgr<ShaderSource>());
    registry.Register(Texture2D::GetStaticType(), new RenderResourceMgr<Texture2D>());
    registry.Register(Material::GetStaticType(), new RenderResourceMgr<Material>());
    registry.Register(Model::GetStaticType(), new RenderResourceMgr<Model>());
}

void Renderer::ProcessCmds()
{
    while(!m_Finished)
    {
        m_TextureCmdQueue.TryProcessNext();
        m_BufferCmdQueue.TryProcessNext();
    }
}
