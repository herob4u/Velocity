#include "vctPCH.h"
#include "Image.h"

#include "Engine/IO/MemoryStream.h"
#include "Engine/Utils/StringUtils.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <sstream>

static std::string FORMAT_PNG = "png";
static std::string FORMAT_TGA = "tga";
static std::string FORMAT_BMP = "bmp";
static std::string FORMAT_JPEG = "jpeg";
static std::string FORMAT_JPG = "jpg";


ImageFormat Image::GetImageFormat(const std::string& filepath)
{
    std::string ext;
    ext.reserve(3);
    Path::GetExtension(filepath, ext);

    if(String::Equals(ext, FORMAT_PNG, false)) { return ImageFormat::PNG; }
    else if(String::Equals(ext, FORMAT_TGA, false)) { return ImageFormat::TGA; }
    else if(String::Equals(ext, FORMAT_BMP, false)) { return ImageFormat::BMP; }
    else if(String::Equals(ext, FORMAT_JPEG, false)) { return ImageFormat::JPG; }
    else if(String::Equals(ext, FORMAT_JPG, false)) { return ImageFormat::JPG; }

    return ImageFormat::UNKNOWN;
}

ImageFormat Image::GetImageFormat(const Path& filepath)
{
    std::string ext;
    ext.reserve(4);
    filepath.GetExtension(ext);

    if(String::Equals(ext, FORMAT_PNG, false)) { return ImageFormat::PNG; }
    else if(String::Equals(ext, FORMAT_TGA, false)) { return ImageFormat::TGA; }
    else if(String::Equals(ext, FORMAT_BMP, false)) { return ImageFormat::BMP; }
    else if(String::Equals(ext, FORMAT_JPEG, false)) { return ImageFormat::JPG; }
    else if(String::Equals(ext, FORMAT_JPG, false)) { return ImageFormat::JPG; }

    return ImageFormat::UNKNOWN;
}

Image::Image(const std::string& filePath)
{
    m_Format = GetImageFormat(filePath);

    stbi_set_flip_vertically_on_load(1);
    stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
    CalculateDepth();
}

Image::Image(const Path& filePath)
{
    m_Format = GetImageFormat(filePath);

    stbi_set_flip_vertically_on_load(1);
    stbi_load(filePath.GetFullPathRef(), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
    CalculateDepth();
}

Image::Image(const void* const sourceBuffer, size_t numBytes, ImageFormat asFormat)
{
    switch(asFormat)
    {
        case ImageFormat::TGA:
            LoadTGA(sourceBuffer, numBytes);
        break;

        case ImageFormat::PNG:
            LoadPNG(sourceBuffer, numBytes);
        break;

        case ImageFormat::BMP:
            LoadBMP(sourceBuffer, numBytes);
        break;

        case ImageFormat::JPG:
            LoadJPG(sourceBuffer, numBytes);
        break;
    }
}

Image::~Image()
{
    Release();
}

void Image::Release()
{
    if(m_Data)
    {
        stbi_image_free(m_Data);
        m_Data = nullptr;
    }
}

// Disable padding so that fread(sizeof(TGAHeader)) works as expected.
#pragma pack(1)
struct TGAHeader
{
    uint8_t IdLength;
    uint8_t ColorMapType;
    uint8_t DataTypeCode; // Image Type
    short ColorMapOrigin;
    short ColorMapLength;
    uint8_t ColorMapDepth;
    short OriginX;
    short OriginY;
    short Width;
    short Height;
    uint8_t BPP;
    uint8_t Descriptor;

    std::string ToString()
    {
        std::stringstream ss;
        ss << "Width: " << Width << '\n'
           << "Height: " << Height << '\n'
           << "BPP: " << (short)BPP << '\n'
           << "OriginXY: " << "(" << OriginX << ", " << OriginY << ")" << '\n'
           << "ColorMapDepth: " << (short)ColorMapDepth << '\n';

        return ss.str();
    }
};
#pragma pack()

void Image::LoadTGA(const void* const buffer, size_t numBytes)
{
    // Create an InputMemoryStream!
    //InputMemoryStream ibuffer(buffer, numBytes);

    //TGAHeader header;
    //ibuffer.Read(&header, sizeof(header));

    //PixelBuffer pixels = (PixelBuffer)malloc(numBytes - header.IdLength);
    //memcpy(pixels, const_cast<void*>(buffer), numBytes);
    // Process header
    // ...
    //VCT_INFO(header.ToString());
    stbi_set_flip_vertically_on_load(1);
    const PixelBuffer imgBuffer = (PixelBuffer)const_cast<void*>(buffer);
    m_Data = stbi_load_from_memory(imgBuffer, numBytes, &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
    VCT_INFO("TGA Image Data at address: {0}", (void*)m_Data);
}

void Image::LoadPNG(const void* const buffer, size_t numBytes)
{
}

void Image::LoadBMP(const void* const buffer, size_t numBytes)
{
}

void Image::LoadJPG(const void* const buffer, size_t numBytes)
{
}

void Image::CalculateDepth()
{
    if(!m_Data)
    {
        ASSERT(m_Width > 0 && m_Height > 0, "Invalid image size - is image corrupt?");
        m_Depth = 0;
    }

    if(stbi_is_16_bit_from_memory(m_Data, (m_Width * m_Height)))
    {
        m_Depth = 16;
    }
    else
    {
        m_Depth = 8;
    }
}
