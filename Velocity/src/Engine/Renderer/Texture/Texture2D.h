#pragma once

#include "Texture.h"
#include "Engine/Resources/Resource.h"

namespace Vct
{
    static Resource::Type ResType_Texture("texture");

    class Image;

    class Texture2D : public Resource, public Texture
    {
    public:
        friend class Renderer;

        static Texture2D* Allocate(uint16_t width, uint16_t height, Format format, DataType type, WrapMode wrapMode = WrapMode::REPEAT);
        static Texture2D* Allocate(const Texture2D::Params& Params);

        // Creates a default placeholder texture
        Texture2D();

        // Creates a texture from an asset file path
        Texture2D(const std::string& filePath);
        Texture2D(const Path& filePath);

        ~Texture2D();

        // Rendering Methods
        void Bind() const;
        void Unbind() const;
        void SetTextureSlot(uint32_t slotId);
        void SetWrapMode(WrapMode wrapMode);

        virtual void Destroy() override;
        Ref<Image> RenderToImage() const;

        uint32_t GetTextureSlot() const { return m_TextureSlot; }

        RES_TYPE(ResType_Texture);
    protected:
        Texture2D(const Texture2D::Params& Params);
        Texture2D(uint16_t width, uint16_t height, Format format, DataType type, WrapMode wrapMode);

        void Rebuild();

        // Resource Interface
        virtual bool Load(const void* rawBinary, size_t bytes) override;
        virtual void Unload() override;

    private:
        // Image Ptr
        std::unique_ptr<class Image> m_Image;
        uint32_t m_TextureSlot;
        Texture2D::Params m_Params;
    };
}