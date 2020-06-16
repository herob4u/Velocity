#include "vctPCH.h"

#include "Renderer.h"

#include "Texture/Texture.h"

#include "glad/glad.h"

using namespace Vct;

Renderer::Renderer()
    : m_Finished(false)
{
    //m_WorkerThread = std::thread(&Renderer::ProcessCmds, this);
}

Renderer::~Renderer()
{
    //m_Finished = true;
    //m_WorkerThread.join();
}

void Renderer::BeginScene(const Camera& camera)
{
    m_TextureCmdQueue.Flush();
    m_BufferCmdQueue.Flush();
}

void Renderer::EndScene()
{
}

void Renderer::GenerateTextureAsync(uint32_t& texId, const uint8_t* data, uint16_t width, uint16_t height, uint16_t format, uint16_t type, uint16_t wrapMode)
{
   RenderCmd* cmd = new GenTexturesCmd(texId, data, width, height, format, type, wrapMode);
   m_TextureCmdQueue.Enqueue(cmd);
}

void Renderer::DeleteTextureAsync(uint32_t & texId)
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

void Renderer::DeleteTexture(uint32_t& texId)
{
    glDeleteTextures(1, &texId);
}

void Renderer::GenerateFramebuffer(Framebuffer& fb) const
{
    ASSERT(fb.m_BufferId == 0, "Framebuffer already occupied");
    glGenFramebuffers(1, &fb.m_BufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, fb.m_BufferId);

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

    glFramebufferTexture2D(GL_FRAMEBUFFER, glAttachment, GL_TEXTURE_2D, fb.m_Texture->GetTextureId(), 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        ASSERT(false, "Framebuffer incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DeleteFramebuffer(Framebuffer& fb) const
{
    glDeleteFramebuffers(1, &fb.m_BufferId);
}

Renderer& Renderer::Get()
{
    static Renderer self;
    return self;
}

void Renderer::ProcessCmds()
{
    while(!m_Finished)
    {
        m_TextureCmdQueue.TryProcessNext();
        m_BufferCmdQueue.TryProcessNext();
    }
}
