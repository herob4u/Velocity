#include "vctPCH.h"
#include "Cubemap.h"

#include "Image.h"

#include "glad/glad.h"
using namespace Vct;

static int GetGLFormat(Texture::Format format)
{
    switch(format)
    {
    case Texture::Format::GREYSCALE:        return GL_RED;
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

Cubemap* Cubemap::Allocate(uint16_t width, uint16_t height, Format format, DataType dataType, WrapMode wrapMode)
{
    Cubemap* cubemap = new Cubemap();

    const int glFormat      = GetGLFormat(format);
    const int glType        = GetGLDataType(dataType);
    const int glWrapMode    = GetGLWrapMode(wrapMode);

    glGenTextures(1, &cubemap->RendererId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->RendererId);

    for(int i = 0; i < CubemapFace::MAX_VAL; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glFormat, width, height, 0, glFormat, glType, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, glWrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, glWrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, glWrapMode);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return cubemap;
}

Cubemap::Cubemap(const Path& right, const Path& left, const Path& top, const Path& bottom, const Path& back, const Path& front)
{
    Image img_right     = Image(right);
    Image img_left      = Image(left);
    Image img_top       = Image(top);
    Image img_bottom    = Image(bottom);
    Image img_back      = Image(back);
    Image img_front     = Image(front);

    InitializeCubemap(std::vector<Image*>{&img_right, &img_left, &img_top, &img_bottom, &img_back, &img_front});
}

Cubemap::~Cubemap()
{
    Destroy();
}

std::vector<Ref<Image>> Cubemap::RenderToImages() const
{
    ASSERT(RendererId != 0, "Texture2D not initialized");

    std::vector<Ref<Image>> outImages;
    outImages.reserve(CubemapFace::MAX_VAL);

    for(int i = 0; i < CubemapFace::MAX_VAL; i++)
    {
        const Params& params = m_Params[i];

        ASSERT(params.DataType != DataType::FLOAT, "Can only render 8-bit depth textures");

        size_t imgSize = params.Width * params.Height * sizeof(uint8_t) * 3;
        void* pixelBuffer = calloc(imgSize, sizeof(uint8_t));

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glGetTextureImage(RendererId, 0, GetGLFormat(params.Format), GetGLDataType(params.DataType), (GLsizei)imgSize, pixelBuffer);

        PixelBuffer imgbuffer = (PixelBuffer)pixelBuffer;

        outImages.emplace_back(Ref<Image>(Image::Acquire(params.Width, params.Height, pixelBuffer)));
    }

    return outImages;
}

void Cubemap::Destroy()
{
    if(RendererId != 0)
    {
        glDeleteTextures(1, &RendererId);
        RendererId = 0;
    }
}

void Cubemap::InitializeCubemap(const std::vector<Image*> faces)
{
    if(faces.size() != 6)
    {
        ASSERT(false, "Expected 6 faces for cubemap");
        return;
    }

    Destroy();

    glGenTextures(1, &RendererId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, RendererId);

    for(int i = 0; i < faces.size(); i++)
    {
        const Image* img = faces[i];
        const int glFormat = img->GetFormat() == ImageFormat::HDR ? GL_RGB16F : GL_RGB;
        const int glType   = img->GetFormat() == ImageFormat::HDR ? GL_FLOAT  : GL_UNSIGNED_BYTE;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glFormat, img->GetWidth(), img->GetHeight(), 0, glFormat, glType, img->GetData());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
