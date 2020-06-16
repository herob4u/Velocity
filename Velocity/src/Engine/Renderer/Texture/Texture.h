#pragma once

#include "Engine/Resources/Resource.h"

static Resource::Type ResType_Texture("texture");

class Texture : public Resource
{
public:
    enum class Format
    {
        R,
        RG,
        RGB,
        RGBA,
        FLOAT,
        DEPTH_STENCIL
    };

    enum class DataType
    {
        UNSIGNED_BYTE,
        FLOAT,
        INT,
        DEPTH_STENCIL
    };

    enum WrapMode
    {
        REPEAT,
        CLAMP,
        MIRROR
    };

    static Texture* Allocate(uint16_t width, uint16_t height, Format format, DataType type, WrapMode wrapMode = WrapMode::REPEAT);

    // Creates a default placeholder texture
    Texture();

    // Creates a texture from an asset file path
    Texture(const std::string& filePath);
    Texture(const Path& filePath);

    // Rendering Methods
    void Bind() const;
    void Unbind() const;
    void SetTextureSlot(uint32_t slotId);
    void SetWrapMode(WrapMode wrapMode);

    void Destroy();

    uint32_t GetTextureSlot() const { return m_TextureSlot; }
    uint32_t GetTextureId() const   { return m_TextureId; }

    RES_TYPE(ResType_Texture);
protected:
    void GLInit();

    // Resource Interface
    virtual bool Load(const void* rawBinary, size_t bytes) override;
    virtual void Unload() override;

private:
    // Image Ptr
    std::unique_ptr<class Image> m_Image;
    uint32_t m_TextureId;
    uint32_t m_TextureSlot;
    WrapMode m_WrapMode;
};