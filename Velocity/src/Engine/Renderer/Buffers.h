#pragma once

class Texture;
namespace Vct
{
    /***********************************************************************/
    /*                          OPENGL BUFFERS                             */    
    /***********************************************************************/

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

        Framebuffer* CreateColorBuffer(uint8_t attachmentSlot, const FramebufferParams& fbParams);
        Framebuffer* CreateDepthBuffer(const FramebufferParams& fbParams);
        Framebuffer* CreateStencilBuffer(const FramebufferParams& fbParams);
        Framebuffer* CreateDepthStencilBuffer(const FramebufferParams& fbParams);

        ~Framebuffer();

        FORCEINLINE uint16_t GetWidth() const   { return m_Params.Width; }
        FORCEINLINE uint16_t GetHeight() const  { return m_Params.Height; }
        FORCEINLINE uint8_t  GetAttachmentSlot() const { return m_AttachmentSlot; }

        void Rebuild();
        void Destroy();
    protected:
        Framebuffer(const FramebufferParams& fbParams);
    private:
        uint32_t m_BufferId;
        Texture* m_Texture;

        FramebufferParams m_Params;
        uint8_t m_AttachmentSlot;
        Type m_Type;
    };
}