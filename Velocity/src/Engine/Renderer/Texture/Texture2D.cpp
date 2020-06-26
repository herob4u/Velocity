#include "vctPCH.h"
#include "Texture2D.h"

#include "Image.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Application.h"

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

static int GetGLFormat(Texture2D::Format format)
{
    switch(format)
    {
        case Texture2D::Format::GREYSCALE:        return GL_RED;
        case Texture2D::Format::RG:               return GL_RG;
        case Texture2D::Format::RGB:              return GL_RGB;
        case Texture2D::Format::RGBA:             return GL_RGBA;
        case Texture2D::Format::FLOAT:            return GL_RGB16F;
        case Texture2D::Format::DEPTH_STENCIL:    return GL_DEPTH24_STENCIL8;
        default: return GL_RGBA;
    }
}

static int GetGLDataType(Texture2D::DataType dataType)
{
    switch(dataType)
    {
        case Texture2D::DataType::UNSIGNED_BYTE:  return GL_UNSIGNED_BYTE;
        case Texture2D::DataType::FLOAT:          return GL_FLOAT;
        case Texture2D::DataType::INT:            return GL_UNSIGNED_INT;
        case Texture2D::DataType::DEPTH_STENCIL:  return GL_UNSIGNED_INT_24_8;
        default:                                return GL_UNSIGNED_BYTE;
    }
}

static int GetGLWrapMode(Texture2D::WrapMode wrapMode)
{
    switch(wrapMode)
    {
        case Texture2D::WrapMode::CLAMP:  return GL_CLAMP_TO_EDGE;
        case Texture2D::WrapMode::REPEAT: return GL_REPEAT;
        case Texture2D::WrapMode::MIRROR: return GL_MIRRORED_REPEAT;
        default:                        return GL_REPEAT;
    }
}

static void GetFormatAndDataType(ImageFormat imgFormat, int channels, Texture2D::Format& outFormat, Texture2D::DataType& outDataType)
{
    if(channels == 3)
    {
        // Could be ordinary 8-bit RGB image, or 3 channel HDR
        if(imgFormat == ImageFormat::HDR)
        {
            outFormat   = Texture2D::Format::FLOAT;
            outDataType = Texture2D::DataType::FLOAT;
        }
        else
        {
            outFormat   = Texture2D::Format::RGB;
            outDataType = Texture2D::DataType::UNSIGNED_BYTE;
        }
    }
    else if(channels == 4)
    {
        outFormat   = Texture2D::Format::RGBA;
        outDataType = Texture2D::DataType::UNSIGNED_BYTE;
    }
    else if(channels == 1)
    {
        // Grayscale
        outFormat   = Texture2D::Format::GREYSCALE;
        outDataType = Texture2D::DataType::UNSIGNED_BYTE;
    }
    else
    {
        ASSERT(false, "Unsupported image format");
    }
}

Texture2D* Texture2D::Allocate(uint16_t width, uint16_t height, Format format, DataType type, Texture2D::WrapMode wrapMode)
{
    Texture2D* texture = new Texture2D(width, height, format, type, wrapMode);
    texture->m_Image.reset(nullptr);
    texture->m_TextureSlot = 0;

    int glFormat    = GetGLFormat(format);
    int glDataType  = GetGLDataType(type);
    int glWrapMode  = GetGLWrapMode(wrapMode);

    Renderer::GenerateTexture(texture->RendererId, nullptr, width, height, glFormat, glDataType, glWrapMode);

    return texture;
}

Texture2D* Texture2D::Allocate(const Texture2D::Params& textureParams)
{
    return Allocate(textureParams.Width, textureParams.Height, textureParams.Format, textureParams.DataType, textureParams.WrapMode);
}

Texture2D::Texture2D()
    : Resource(s_DefaultTexturePath)
    , m_TextureSlot(GL_TEXTURE0)
{
    //m_Image = std::make_unique<Image>(s_DefaultTexturePath);
}

Texture2D::Texture2D(const std::string& filePath)
    : Resource(filePath)
    , m_TextureSlot(GL_TEXTURE0)
{
    //m_Image = std::make_unique<Image>(filePath);
}

Texture2D::Texture2D(const Path& filePath)
    : Resource(filePath)
    , m_TextureSlot(GL_TEXTURE0)
{
    //m_Image = std::make_unique<Image>(filePath);
}

Texture2D::~Texture2D()
{
    Destroy();

    m_Image.reset(nullptr);
}

void Texture2D::Bind(uint32_t textureSlot)
{
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_2D, RendererId);
}

void Texture2D::Unbind(uint32_t textureSlot)
{
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::SetTextureSlot(uint32_t slotId)
{
    ASSERT((slotId + GL_TEXTURE0) <= (GL_TEXTURE31), "GL texture slot out of range");
    m_TextureSlot = GL_TEXTURE0 + slotId;
}

void Texture2D::SetWrapMode(WrapMode wrapMode)
{
    // Texture2D MUST be bound!
    const int glWrapMode = GetGLWrapMode(wrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapMode);
}

void Texture2D::Destroy()
{
    if(RendererId != 0)
    {
        Renderer::DeleteTexture(*this);
        RendererId = 0;
    }
}

Ref<Image> Texture2D::RenderToImage() const
{
    ASSERT(RendererId != 0, "Texture2D not initialized");
    ASSERT(m_Params.DataType != DataType::FLOAT, "Can only render 8-bit depth textures");

    size_t imgSize = m_Params.Width * m_Params.Height * sizeof(uint8_t) * 3;
    void* pixelBuffer = calloc(imgSize, sizeof(uint8_t));

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glGetTextureImage(RendererId, 0, GetGLFormat(m_Params.Format), GetGLDataType(m_Params.DataType), (GLsizei)imgSize, pixelBuffer);

    PixelBuffer imgbuffer = (PixelBuffer)pixelBuffer;

    return Ref<Image>(Image::Acquire(m_Params.Width, m_Params.Height, pixelBuffer));
}

Texture2D::Texture2D(const Texture2D::Params& textureParams)
    : Resource(StringId::NONE)
    , m_TextureSlot(GL_TEXTURE0)
    , m_Params(textureParams)
{
}

Texture2D::Texture2D(uint16_t width, uint16_t height, Format format, DataType type, WrapMode wrapMode)
    : Resource(StringId::NONE)
    , m_TextureSlot(GL_TEXTURE0)
{
    m_Params.Width = width;
    m_Params.Height = height;
    m_Params.Format = format;
    m_Params.DataType = type;
    m_Params.WrapMode = wrapMode;
}

void Texture2D::Rebuild()
{
    // 6408 for RGBA
    const int format    = GetGLFormat(m_Params.Format);
    const int type      = GetGLDataType(m_Params.DataType);
    const int wrapMode  = GetGLWrapMode(m_Params.WrapMode);

    Vct::Renderer& renderer = Vct::Renderer::Get();

    if(std::this_thread::get_id() == MAIN_THREAD_ID)
    {
        Init(m_Image->GetData(), m_Image->GetWidth(), m_Image->GetHeight(), format, type, wrapMode);
    }
    else
    {
        renderer.GenerateTextureAsync(RendererId, m_Image->GetData(), m_Image->GetWidth(), m_Image->GetHeight(), format, type, wrapMode);
    }
}

void Texture2D::Init(const uint8_t * data, uint16_t width, uint16_t height, uint16_t format, uint16_t type, uint16_t wrapMode)
{
    glGenTextures(1, &RendererId);
    glBindTexture(GL_TEXTURE_2D, RendererId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture2D::Load(const void* rawBinary, size_t bytes)
{
    ImageFormat format = Image::GetImageFormat(GetPath());
    m_Image.reset(new Image(rawBinary, bytes, format));

    GetFormatAndDataType(format, m_Image->GetNumChannels(), m_Params.Format, m_Params.DataType);
    Rebuild();

    return true;
}

void Texture2D::Unload()
{
    Vct::Renderer& renderer = Vct::Renderer::Get();

    if(std::this_thread::get_id() == MAIN_THREAD_ID)
    {
        if(RendererId != 0)
            glDeleteTextures(1, &RendererId);
    }
    else
    {
        renderer.DeleteTextureAsync(RendererId);
    }

    m_Image.release();
}
