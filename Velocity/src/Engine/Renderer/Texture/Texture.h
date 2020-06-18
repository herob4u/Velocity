#pragma once

#include "Engine/Resources/Resource.h"

static Resource::Type ResType_Texture("texture");

namespace Vct
{
    class Image;
    class Texture;

    class Texture : public Resource
    {
    public:
        friend class Renderer;

        enum class Format : uint8_t
        {
            GREYSCALE,
            RG,
            RGB,
            RGBA,
            FLOAT,
            DEPTH_STENCIL
        };

        enum class DataType : uint8_t
        {
            UNSIGNED_BYTE,
            FLOAT,
            INT,
            DEPTH_STENCIL
        };

        enum WrapMode : uint8_t
        {
            REPEAT,
            CLAMP,
            MIRROR
        };

        struct Params
        {
            uint16_t Width = 256;
            uint16_t Height = 256;
            Texture::Format Format = Texture::Format::RGBA;
            Texture::DataType DataType = Texture::DataType::UNSIGNED_BYTE;
            Texture::WrapMode WrapMode = Texture::WrapMode::REPEAT;
        };

        static Texture* Allocate(uint16_t width, uint16_t height, Format format, DataType type, WrapMode wrapMode = WrapMode::REPEAT);
        static Texture* Allocate(const Texture::Params& Params);

        // Creates a default placeholder texture
        Texture();

        // Creates a texture from an asset file path
        Texture(const std::string& filePath);
        Texture(const Path& filePath);

        ~Texture();

        // Rendering Methods
        void Bind() const;
        void Unbind() const;
        void SetTextureSlot(uint32_t slotId);
        void SetWrapMode(WrapMode wrapMode);

        void Destroy();
        Ref<Image> RenderToImage() const;

        uint32_t GetTextureSlot() const { return m_TextureSlot; }
        uint32_t GetRendererId() const   { return m_TextureId; }

        RES_TYPE(ResType_Texture);
    protected:
        Texture(const Texture::Params& Params);
        Texture(uint16_t width, uint16_t height, Format format, DataType type, WrapMode wrapMode);

        void Rebuild();

        // Resource Interface
        virtual bool Load(const void* rawBinary, size_t bytes) override;
        virtual void Unload() override;

    private:
        // Image Ptr
        std::unique_ptr<class Image> m_Image;
        uint32_t m_TextureId;
        uint32_t m_TextureSlot;
        Texture::Params m_Params;
    };
}