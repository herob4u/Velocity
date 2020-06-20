#pragma once

namespace Vct
{
    /***********************************************************************/
    /*                          OPENGL BUFFERS                             */    
    /***********************************************************************/

    class Texture;
    class Cubemap;

    struct FramebufferParams
    {
        uint16_t Width  = 256;
        uint16_t Height = 256;
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

        FORCEINLINE uint16_t GetWidth() const   { return m_Params.Width; }
        FORCEINLINE uint16_t GetHeight() const  { return m_Params.Height; }
        FORCEINLINE uint8_t  GetAttachmentSlot() const  { return m_AttachmentSlot; }
        FORCEINLINE uint32_t GetRendererId() const      { return m_RendererId; }
        FORCEINLINE Texture* GetTextureTarget() const   { return m_Texture; }

        void Rebuild();
        void Destroy();
    protected:
        Framebuffer(const FramebufferParams& fbParams);
    private:
        uint32_t m_RendererId;
        Texture* m_Texture;

        FramebufferParams m_Params;
        uint8_t m_AttachmentSlot;
        Type m_Type;
    };
}