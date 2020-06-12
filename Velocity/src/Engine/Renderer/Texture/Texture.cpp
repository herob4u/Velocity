#include "vctPCH.h"
#include "Texture.h"

#include "Image.h"

#include "Engine/Renderer/Renderer.h"

#include "glad/glad.h"

static const Path s_DefaultTexturePath("T_NullTexture.png");
//static Resource::Type ResType_Texture = Resource::Type("texture");

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

void Texture::SetWrapMode(TextureWrapMode wrapMode)
{
    // Texture MUST be bound!
    const int glWrapMode = wrapMode == TextureWrapMode::REPEAT ? GL_REPEAT
                         : wrapMode == TextureWrapMode::CLAMP ? GL_CLAMP_TO_EDGE
                         : GL_MIRRORED_REPEAT;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapMode);
}

void Texture::GLInit()
{
    // 6408 for RGBA
    const int format = GetGLFormat(m_Image.get());
    Vct::Renderer& renderer = Vct::Renderer::Get();

    renderer.GenerateTexture(m_TextureId, m_Image->GetData(), m_Image->GetWidth(), m_Image->GetHeight(), format, GL_REPEAT);
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

    renderer.DeleteTexture(m_TextureId);

    m_Image.release();
}
