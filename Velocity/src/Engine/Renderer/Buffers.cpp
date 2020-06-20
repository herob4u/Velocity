#include "vctPCH.h"
#include "Buffers.h"
#include "Renderer.h"

#include "Texture/Texture2D.h"
#include "Texture/Cubemap.h"

using namespace Vct;


void Framebuffer::Rebuild()
{
    Renderer::GenerateFramebuffer(*this);
}

void Framebuffer::Destroy()
{
    Renderer::DeleteFramebuffer(*this);
}

Framebuffer::Framebuffer(const FramebufferParams& fbParams)
    : m_Params(fbParams)
    , m_RendererId(0)
{
}

Framebuffer* Framebuffer::CreateColorBuffer(uint8_t attachmentSlot, const FramebufferParams& fbParams)
{
    Framebuffer* fb         = new Framebuffer(fbParams);
    fb->m_AttachmentSlot    = attachmentSlot;
    fb->m_Type              = Type::COLOR;
    fb->m_Texture           = Texture2D::Allocate(fb->GetWidth(), fb->GetHeight(), Texture2D::Format::RGB, Texture2D::DataType::UNSIGNED_BYTE);

    fb->Rebuild();

    return fb;
}

Framebuffer* Framebuffer::CreateDepthBuffer(const FramebufferParams& fbParams)
{
    Framebuffer* fb         = new Framebuffer(fbParams);
    fb->m_AttachmentSlot    = 0;
    fb->m_Type              = Type::DEPTH;
    fb->m_Texture           = Texture2D::Allocate(fb->GetWidth(), fb->GetHeight(), Texture2D::Format::RGBA, Texture2D::DataType::UNSIGNED_BYTE);

    fb->Rebuild();

    return fb;
}

Framebuffer* Framebuffer::CreateStencilBuffer(const FramebufferParams& fbParams)
{
    Framebuffer* fb         = new Framebuffer(fbParams);
    fb->m_AttachmentSlot    = 0;
    fb->m_Type              = Type::STENCIL;
    fb->m_Texture           = Texture2D::Allocate(fb->GetWidth(), fb->GetHeight(), Texture2D::Format::RGBA, Texture2D::DataType::UNSIGNED_BYTE);

    fb->Rebuild();

    return fb;
}

Framebuffer* Framebuffer::CreateDepthStencilBuffer(const FramebufferParams& fbParams)
{
    Framebuffer* fb         = new Framebuffer(fbParams);
    fb->m_AttachmentSlot    = 0;
    fb->m_Type              = Type::DEPTH_STENCIL;
    fb->m_Texture           = Texture2D::Allocate(fb->GetWidth(), fb->GetHeight(), Texture2D::Format::DEPTH_STENCIL, Texture2D::DataType::DEPTH_STENCIL);

    fb->Rebuild();

    return fb;
}

Framebuffer::~Framebuffer()
{
    if(m_Texture)
    {
        m_Texture->Destroy();
        delete m_Texture;
        m_Texture = nullptr;
    }
}
