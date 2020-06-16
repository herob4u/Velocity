#include "vctPCH.h"
#include "Texture.h"

#include "Image.h"

#include "Engine/Renderer/Renderer.h"

#include "glad/glad.h"

static const Path s_DefaultTexturePath("T_NullTexture.png");

static int GetGLFormat(const Image* img)
{
    ASSERT(img, "Invalid image");
    if(!img)
        return GL_RGBA;

    switch(img->GetNumChannels())
    {
        case 1: return GL_RED; // To be interpreted in fragment shader
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: ASSERT(false, "Invalid number of channels");
    }

    return GL_RGBA;
}

static int GetGLFormat(Texture::Format format)
{
    switch(format)
    {
        case Texture::Format::R:                return GL_RED;
        case Texture::Format::RG:               return GL_RG;
        case Texture::Format::RGB:              return GL_RGB;
        case Texture::Format::RGBA:             return GL_RGBA;
        case Texture::Format::FLOAT:            return GL_RGB16F;
        case Texture::Format::DEPTH_STENCIL:    return GL_DEPTH24_STENCIL8;
        default: return GL_RGBA;
    }
}

static int GetGLDataType(Texture::DataType dataType)
{
    switch(dataType)
    {
        case Texture::DataType::UNSIGNED_BYTE:  return GL_UNSIGNED_BYTE;
        case Texture::DataType::FLOAT:          return GL_FLOAT;
        case Texture::DataType::INT:            return GL_UNSIGNED_INT;
        case Texture::DataType::DEPTH_STENCIL:  return GL_UNSIGNED_INT_24_8;
        default:                                return GL_UNSIGNED_BYTE;
    }
}

static int GetGLWrapMode(Texture::WrapMode wrapMode)
{
    switch(wrapMode)
    {
        case Texture::WrapMode::CLAMP:  return GL_CLAMP_TO_EDGE;
        case Texture::WrapMode::REPEAT: return GL_REPEAT;
        case Texture::WrapMode::MIRROR: return GL_MIRRORED_REPEAT;
        default:                        return GL_REPEAT;
    }
}

Texture* Texture::Allocate(uint16_t width, uint16_t height, Format format, DataType type, Texture::WrapMode wrapMode)
{
    Texture* texture = new Texture(StringId::NONE);
    texture->m_Image.reset(nullptr);
    texture->m_TextureSlot = 0;
    texture->m_WrapMode = wrapMode;

    int glFormat    = GetGLFormat(format);
    int glDataType  = GetGLDataType(type);
    int glWrapMode  = GetGLWrapMode(wrapMode);

    Vct::Renderer& renderer = Vct::Renderer::Get();

    renderer.GenerateTexture(texture->m_TextureId, nullptr, width, height, glFormat, glDataType, glWrapMode);

    return texture;
}

Texture::Texture()
    : Resource(s_DefaultTexturePath)
    , m_TextureId(0)
    , m_TextureSlot(GL_TEXTURE0)
    , m_WrapMode(WrapMode::REPEAT)
{
    //m_Image = std::make_unique<Image>(s_DefaultTexturePath);
}

Texture::Texture(const std::string& filePath)
    : Resource(filePath)
    , m_TextureId(0)
    , m_TextureSlot(GL_TEXTURE0)
    , m_WrapMode(WrapMode::REPEAT)
{
    //m_Image = std::make_unique<Image>(filePath);
}

Texture::Texture(const Path& filePath)
    : Resource(filePath)
    , m_TextureId(0)
    , m_TextureSlot(GL_TEXTURE0)
    , m_WrapMode(WrapMode::REPEAT)
{
    //m_Image = std::make_unique<Image>(filePath);
}

void Texture::Bind() const
{
    glActiveTexture(m_TextureSlot);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetTextureSlot(uint32_t slotId)
{
    ASSERT((slotId + GL_TEXTURE0) <= (GL_TEXTURE31), "GL texture slot out of range");
    m_TextureSlot = GL_TEXTURE0 + slotId;
}

void Texture::SetWrapMode(WrapMode wrapMode)
{
    // Texture MUST be bound!
    const int glWrapMode = GetGLWrapMode(wrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapMode);
}

void Texture::Destroy()
{
    Vct::Renderer& renderer = Vct::Renderer::Get();
    renderer.DeleteTexture(m_TextureId);
}

void Texture::GLInit()
{
    // 6408 for RGBA
    const int format    = GetGLFormat(m_Image.get());
    const int wrapMode  = GetGLWrapMode(m_WrapMode);

    Vct::Renderer& renderer = Vct::Renderer::Get();

    renderer.GenerateTextureAsync(m_TextureId, m_Image->GetData(), m_Image->GetWidth(), m_Image->GetHeight(), format, wrapMode);
    /*
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Image->GetWidth(), m_Image->GetHeight(), 0, format, GL_UNSIGNED_BYTE, m_Image->GetData());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    */
}

bool Texture::Load(const void* rawBinary, size_t bytes)
{
    ImageFormat format = Image::GetImageFormat(GetPath());
    m_Image.reset(new Image(rawBinary, bytes, format));

    GLInit();

    return true;
}

void Texture::Unload()
{
    //glDeleteTextures(1, &m_TextureId);
    Vct::Renderer& renderer = Vct::Renderer::Get();

    renderer.DeleteTextureAsync(m_TextureId);

    m_Image.release();
}
