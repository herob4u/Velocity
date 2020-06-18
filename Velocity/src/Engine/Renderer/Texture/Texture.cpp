#include "vctPCH.h"
#include "Texture.h"

#include "Image.h"

#include "Engine/Renderer/Renderer.h"

#include "glad/glad.h"

static const Path s_DefaultTexturePath("T_NullTexture.png");

using namespace Vct;

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

static void GetFormatAndDataType(ImageFormat imgFormat, Texture::Format& outFormat, Texture::DataType& outDataType)
{
    switch(imgFormat)
    {
        case ImageFormat::TGA:
        {
            outFormat = Texture::Format::RGBA;
            outDataType = Texture::DataType::UNSIGNED_BYTE;
        }
        break;

        case ImageFormat::PNG:
        {
            outFormat = Texture::Format::RGBA;
            outDataType = Texture::DataType::UNSIGNED_BYTE;
        }
        break;

        case ImageFormat::BMP:
        {
            outFormat = Texture::Format::RGBA;
            outDataType = Texture::DataType::UNSIGNED_BYTE;
        }
        break;

        case ImageFormat::JPG:
        {
            outFormat = Texture::Format::RGB;
            outDataType = Texture::DataType::UNSIGNED_BYTE;
        }
        break;

        case ImageFormat::HDR:
        {
            outFormat = Texture::Format::FLOAT;
            outDataType = Texture::DataType::FLOAT;
        }
        break;

        default: ASSERT(false, "Invalid Image format");
    }
}

Texture* Texture::Allocate(uint16_t width, uint16_t height, Format format, DataType type, Texture::WrapMode wrapMode)
{
    Texture* texture = new Texture(width, height, format, type, wrapMode);
    texture->m_Image.reset(nullptr);
    texture->m_TextureSlot = 0;

    int glFormat    = GetGLFormat(format);
    int glDataType  = GetGLDataType(type);
    int glWrapMode  = GetGLWrapMode(wrapMode);

    Renderer::GenerateTexture(texture->m_TextureId, nullptr, width, height, glFormat, glDataType, glWrapMode);

    return texture;
}

Texture* Texture::Allocate(const Texture::Params& textureParams)
{
    return Allocate(textureParams.Width, textureParams.Height, textureParams.Format, textureParams.DataType, textureParams.WrapMode);
}

Texture::Texture()
    : Resource(s_DefaultTexturePath)
    , m_TextureId(0)
    , m_TextureSlot(GL_TEXTURE0)
{
    //m_Image = std::make_unique<Image>(s_DefaultTexturePath);
}

Texture::Texture(const std::string& filePath)
    : Resource(filePath)
    , m_TextureId(0)
    , m_TextureSlot(GL_TEXTURE0)
{
    //m_Image = std::make_unique<Image>(filePath);
}

Texture::Texture(const Path& filePath)
    : Resource(filePath)
    , m_TextureId(0)
    , m_TextureSlot(GL_TEXTURE0)
{
    //m_Image = std::make_unique<Image>(filePath);
}

Texture::~Texture()
{
    Destroy();

    m_Image.reset(nullptr);
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
    Renderer::DeleteTexture(*this);
}

Ref<Image> Texture::RenderToImage() const
{
    ASSERT(m_TextureId != 0, "Texture not initialized");
    ASSERT(m_Params.DataType != DataType::FLOAT, "Can only render 8-bit depth textures");

    size_t imgSize = m_Params.Width * m_Params.Height * sizeof(uint8_t);
    void* pixelBuffer = calloc(imgSize, sizeof(uint8_t));

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glGetTextureImage(m_TextureId, 0, GetGLFormat(m_Params.Format), GetGLDataType(m_Params.DataType), (GLsizei)imgSize, pixelBuffer);

    PixelBuffer imgbuffer = (PixelBuffer)pixelBuffer;

    VCT_INFO("Pixel: {0}, {1}, {2}", imgbuffer[0], imgbuffer[1], imgbuffer[2]);
    return Ref<Image>(Image::Acquire(m_Params.Width, m_Params.Height, pixelBuffer));
}

Texture::Texture(const Texture::Params& textureParams)
    : Resource(StringId::NONE)
    , m_TextureId(0)
    , m_TextureSlot(GL_TEXTURE0)
    , m_Params(textureParams)
{
}

Texture::Texture(uint16_t width, uint16_t height, Format format, DataType type, WrapMode wrapMode)
    : Resource(StringId::NONE)
    , m_TextureId(0)
    , m_TextureSlot(GL_TEXTURE0)
{
    m_Params.Width = width;
    m_Params.Height = height;
    m_Params.Format = format;
    m_Params.DataType = type;
    m_Params.WrapMode = wrapMode;
}

void Texture::Rebuild()
{
    // 6408 for RGBA
    const int format    = GetGLFormat(m_Params.Format);
    const int type      = GetGLDataType(m_Params.DataType);
    const int wrapMode  = GetGLWrapMode(m_Params.WrapMode);

    Vct::Renderer& renderer = Vct::Renderer::Get();

    renderer.GenerateTextureAsync(m_TextureId, m_Image->GetData(), m_Image->GetWidth(), m_Image->GetHeight(), format, type, wrapMode);
}

bool Texture::Load(const void* rawBinary, size_t bytes)
{
    ImageFormat format = Image::GetImageFormat(GetPath());
    GetFormatAndDataType(format, m_Params.Format, m_Params.DataType);

    m_Image.reset(new Image(rawBinary, bytes, format));

    Rebuild();

    return true;
}

void Texture::Unload()
{
    Vct::Renderer& renderer = Vct::Renderer::Get();

    renderer.DeleteTextureAsync(m_TextureId);

    m_Image.release();
}
