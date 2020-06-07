#pragma once

#include "Engine/Resources/Resource.h"

static Resource::Type ResType_Texture("texture");

enum TextureWrapMode
{
    REPEAT,
    CLAMP,
    MIRROR
};

class Texture : public Resource
{
public:
    // Creates a default placeholder texture
    Texture();

    // Creates a texture from an asset file path
    Texture(const std::string& filePath);
    Texture(const Path& filePath);

    // Rendering Methods
    void Bind() const;
    void Unbind() const;
    void SetTextureSlot(uint32_t slotId);
    void SetWrapMode(TextureWrapMode wrapMode);

    uint32_t GetTextureSlot() const { return m_TextureSlot; }

    RES_TYPE(ResType_Texture);
protected:
    void GLInit();

    // Resource Interface
    virtual bool Load(const void* const rawBinary, size_t bytes) override;
    virtual void Unload() override;

private:
    // Image Ptr
    std::unique_ptr<class Image> m_Image;
    uint32_t m_TextureId;
    uint32_t m_TextureSlot;
};