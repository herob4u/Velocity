#include "vctPCH.h"
#include "Buffers.h"
#include "Renderer.h"

#include "Texture/Texture2D.h"
#include "Texture/Cubemap.h"

#include "glad/glad.h"

using namespace Vct;

static int GetGLType(Framebuffer::Type type)
{
    switch(type)
    {
        case Framebuffer::Type::COLOR :             return GL_COLOR_ATTACHMENT0;
        case Framebuffer::Type::DEPTH:              return GL_DEPTH_ATTACHMENT;
        case Framebuffer::Type::STENCIL:            return GL_STENCIL_ATTACHMENT;
        case Framebuffer::Type::DEPTH_STENCIL:      return GL_DEPTH_STENCIL;

        default :
        {
            ASSERT(false, "Invalid framebuffer type");
            return GL_COLOR_ATTACHMENT0;
        }
    }
}

void Framebuffer::SetCubemapTargetFace(CubemapFace face)
{
    if(Cubemap* cubemap = dynamic_cast<Cubemap*>(m_Texture.get()))
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GetGLType(m_Type), GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cubemap->GetRendererId(), 0 );
    }
}

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

void Framebuffer::AllocateTexture(const TextureTargetParams& textureParams, Type bufferType)
{
    uint16_t width  = textureParams.Width;
    uint16_t height = textureParams.Height;

    if(textureParams.bSizeToFit)
    {
        width   = m_Params.Width;
        height  = m_Params.Height;
    }

    Texture::Format format;
    Texture::DataType dataType;

    switch(bufferType)
    {
        case Type::COLOR:
        {
            format = Texture::Format::RGB;
            dataType = Texture::DataType::UNSIGNED_BYTE;
        }
        break;

        case Type::DEPTH:
        {
            format = Texture::Format::RGBA;
            dataType = Texture::DataType::UNSIGNED_BYTE;
        }
        break;

        case Type::STENCIL:
        {
            format = Texture::Format::RGBA;
            dataType = Texture::DataType::UNSIGNED_BYTE;
        }
        break;

        case Type::DEPTH_STENCIL:
        {
            format = Texture::Format::DEPTH_STENCIL;
            dataType = Texture::DataType::DEPTH_STENCIL;
        }
        break;

        default: ASSERT(false, "Invalid framebuffer type");
    }

    if(textureParams.bIsCubemap)
    {
        AllocateCubemap(width, height, format, dataType);
    }
    else
    {
        AllocateTexture2D(width, height, format, dataType);
    }
}

void Framebuffer::AllocateTexture2D(uint16_t width, uint16_t height, Texture2D::Format format, Texture2D::DataType dataType)
{
    m_Texture.reset(Texture2D::Allocate(width, height, format, dataType));
}

void Framebuffer::AllocateCubemap(uint16_t width, uint16_t height, Texture::Format format, Texture::DataType dataType)
{
    m_Texture.reset(Cubemap::Allocate(width, height, format, dataType));
}

Framebuffer* Framebuffer::CreateColorBuffer(uint8_t attachmentSlot, const FramebufferParams& fbParams)
{
    Framebuffer* fb         = new Framebuffer(fbParams);
    fb->m_AttachmentSlot    = attachmentSlot;
    fb->m_Type              = Type::COLOR;

    fb->AllocateTexture(fbParams.TextureTarget, fb->m_Type);
    fb->Rebuild();

    return fb;
}

Framebuffer* Framebuffer::CreateDepthBuffer(const FramebufferParams& fbParams)
{
    Framebuffer* fb         = new Framebuffer(fbParams);
    fb->m_AttachmentSlot    = 0;
    fb->m_Type              = Type::DEPTH;

    fb->AllocateTexture(fbParams.TextureTarget, fb->m_Type);
    fb->Rebuild();

    return fb;
}

Framebuffer* Framebuffer::CreateStencilBuffer(const FramebufferParams& fbParams)
{
    Framebuffer* fb         = new Framebuffer(fbParams);
    fb->m_AttachmentSlot    = 0;
    fb->m_Type              = Type::STENCIL;

    fb->AllocateTexture(fbParams.TextureTarget, fb->m_Type);
    fb->Rebuild();

    return fb;
}

Framebuffer* Framebuffer::CreateDepthStencilBuffer(const FramebufferParams& fbParams)
{
    Framebuffer* fb         = new Framebuffer(fbParams);
    fb->m_AttachmentSlot    = 0;
    fb->m_Type              = Type::DEPTH_STENCIL;

    fb->AllocateTexture(fbParams.TextureTarget, fb->m_Type);
    fb->Rebuild();

    return fb;
}

Framebuffer::~Framebuffer()
{
    if(m_Texture)
    {
        m_Texture.reset(nullptr);
    }
}
