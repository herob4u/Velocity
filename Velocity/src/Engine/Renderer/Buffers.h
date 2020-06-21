#pragma once

#include "Texture/Texture.h"

namespace Vct
{
    /***********************************************************************/
    /*                          OPENGL BUFFERS                             */    
    /***********************************************************************/

    class Texture;
    class Cubemap;
    enum CubemapFace : uint_fast8_t;

    struct TextureTargetParams
    {
        bool bIsCubemap     = false;
        bool bSizeToFit     = true;
        uint16_t Width      = 256;
        uint16_t Height     = 256;
    };

    struct FramebufferParams
    {
        uint16_t Width  = 256;
        uint16_t Height = 256;
        TextureTargetParams TextureTarget = TextureTargetParams();
    };

    class Framebuffer
    {
    public:
        friend class Renderer;

        enum class Type : uint8_t
        {
            COLOR,
            DEPTH,
            STENCIL,
            DEPTH_STENCIL
        };

        static Framebuffer* CreateColorBuffer(uint8_t attachmentSlot, const FramebufferParams& fbParams);
        static Framebuffer* CreateDepthBuffer(const FramebufferParams& fbParams);
        static Framebuffer* CreateStencilBuffer(const FramebufferParams& fbParams);
        static Framebuffer* CreateDepthStencilBuffer(const FramebufferParams& fbParams);

        ~Framebuffer();

        FORCEINLINE uint16_t        GetWidth() const            { return m_Params.Width; }
        FORCEINLINE uint16_t        GetHeight() const           { return m_Params.Height; }
        FORCEINLINE uint8_t         GetAttachmentSlot() const   { return m_AttachmentSlot; }
        FORCEINLINE uint32_t        GetRendererId() const       { return m_RendererId; }
        FORCEINLINE const Texture*  GetTextureTarget() const    { return m_Texture.get(); }

        void SetCubemapTargetFace(CubemapFace face);
        void Rebuild();
        void Destroy();
    protected:
        Framebuffer(const FramebufferParams& fbParams);

        void AllocateTexture(const TextureTargetParams& params, Type bufferType);
        void AllocateTexture2D(uint16_t width, uint16_t height, Texture::Format format, Texture::DataType dataType);
        void AllocateCubemap(uint16_t width, uint16_t height, Texture::Format format, Texture::DataType dataType);
    private:
        uint32_t m_RendererId;
        std::unique_ptr<Texture> m_Texture;

        FramebufferParams m_Params;
        uint8_t m_AttachmentSlot;
        Type m_Type;
    };
}