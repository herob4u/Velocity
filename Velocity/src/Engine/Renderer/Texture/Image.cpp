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
static std::string FORMAT_HDR = "hdr";

using namespace Vct;

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
    else if(String::Equals(ext, FORMAT_HDR, false)) { return ImageFormat::HDR; }

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
    else if(String::Equals(ext, FORMAT_HDR, false)) { return ImageFormat::HDR; }

    return ImageFormat::UNKNOWN;
}

Image* Image::Acquire(int width, int height, void* sourceBuffer, int channels, int depth)
{
    Image* img = new Image();
    img->m_Data = (PixelBuffer)sourceBuffer;
    img->m_Width = width;
    img->m_Height = height;
    img->m_Channels = channels;
    img->m_Depth = depth;

    return img;
}

Image::Image(const std::string& filePath)
    : m_Data(nullptr)
    , m_Width(-1)
    , m_Height(-1)
    , m_Channels(0)
    , m_Depth(0)
    , m_Format(ImageFormat::UNKNOWN)
{
    m_Format = GetImageFormat(filePath);

    stbi_set_flip_vertically_on_load(1);
    m_Data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
    CalculateDepth();
}

Image::Image(const Path& filePath)
    : m_Data(nullptr)
    , m_Width(-1)
    , m_Height(-1)
    , m_Channels(0)
    , m_Depth(0)
    , m_Format(ImageFormat::UNKNOWN)
{
    m_Format = GetImageFormat(filePath);

    stbi_set_flip_vertically_on_load(1);
    m_Data = stbi_load(filePath.GetFullPathRef(), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
    CalculateDepth();
}

Image::Image(const void* sourceBuffer, size_t numBytes, ImageFormat asFormat)
    : m_Data(nullptr)
    , m_Width(-1)
    , m_Height(-1)
    , m_Channels(0)
    , m_Depth(0)
    , m_Format(asFormat)

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

        case ImageFormat::HDR:
            LoadHDR(sourceBuffer, numBytes);
        break;

        case ImageFormat::RAW:
            LoadRAW(sourceBuffer, numBytes);
        break;

        default: ASSERT(false, "Invalid Image format");
    }

    CalculateDepth();
}

void Image::Write(const std::string& filepath) const
{
    ImageFormat outputFormat = GetImageFormat(filepath);
    stbi_flip_vertically_on_write(1);
    switch(outputFormat)
    {
        case ImageFormat::TGA:
            stbi_write_tga(filepath.c_str(), m_Width, m_Height, m_Channels, m_Data);
        break;

        case ImageFormat::PNG:
            stbi_write_png(filepath.c_str(), m_Width, m_Height, m_Channels, m_Data, 4);
        break;

        case ImageFormat::BMP:
            stbi_write_bmp(filepath.c_str(), m_Width, m_Height, m_Channels, m_Data);
        break;

        case ImageFormat::JPG:
            stbi_write_jpg(filepath.c_str(), m_Width, m_Height, m_Channels, m_Data, 100);
        break;

        case ImageFormat::HDR:
            stbi_write_hdr(filepath.c_str(), m_Width, m_Height, m_Channels, (float*)m_Data);
        break;

        default: ASSERT(false, "Invalid output image format");
    }
}

Image::Image()
    : m_Data(nullptr)
    , m_Width(-1)
    , m_Height(-1)
    , m_Channels(0)
    , m_Depth(0)
    , m_Format(ImageFormat::UNKNOWN)
{
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

        m_Width = 0;
        m_Height = 0;
        m_Channels = 0;
        m_Format = ImageFormat::UNKNOWN;
    }
}

uint8_t Image::PixelAt(int x, int y) const
{
    const size_t stride = ((m_Depth / sizeof(uint8_t)) * m_Channels);
    const size_t idx = ((y -1) * m_Width + x - 1) * stride;

    return m_Data[idx];
}

float Image::PixelAt_f(int x, int y) const
{
    const size_t stride = ((m_Depth / sizeof(uint8_t)) * m_Channels);
    const size_t idx = ((y - 1) * m_Width + x - 1) * stride;

    return ((float*)m_Data)[idx];
}

Image* Image::Extract(int x, int y, int w, int h) const
{
    const size_t stride = ((m_Depth/sizeof(uint8_t)) * m_Channels);
    const size_t idx = ((y - 1) * m_Width + x - 1) * stride;
    
    PixelBuffer start = &m_Data[idx];

    Image* subImg = new Image();
    subImg->m_Channels = m_Channels;
    subImg->m_Depth = m_Depth;
    subImg->m_Format = m_Format;
    subImg->m_Width = w;
    subImg->m_Height = h;
    subImg->m_Data = (PixelBuffer)malloc( stride * w * h );

    memcpy(subImg->m_Data, start, stride * w * h);

    return subImg;
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

void Image::LoadTGA(const void* buffer, size_t numBytes)
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
    const PixelBuffer imgBuffer = (const PixelBuffer)(buffer);
    m_Data = stbi_load_from_memory(imgBuffer, static_cast<int>(numBytes), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
    VCT_INFO("TGA Image Data at address: {0}", (void*)m_Data);
}

void Image::LoadPNG(const void* buffer, size_t numBytes)
{
    stbi_set_flip_vertically_on_load(1);
    const PixelBuffer imgBuffer = (const PixelBuffer)(buffer);
    m_Data = stbi_load_from_memory(imgBuffer, static_cast<int>(numBytes), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
    VCT_INFO("PNG Image Data at address: {0}", (void*)m_Data);
}

void Image::LoadBMP(const void* buffer, size_t numBytes)
{
    stbi_set_flip_vertically_on_load(1);
    const PixelBuffer imgBuffer = (const PixelBuffer)(buffer);
    m_Data = stbi_load_from_memory(imgBuffer, static_cast<int>(numBytes), &m_Width, &m_Height, &m_Channels, STBI_rgb);
    VCT_INFO("BMP Image Data at address: {0}", (void*)m_Data);
}

void Image::LoadJPG(const void* buffer, size_t numBytes)
{
    stbi_set_flip_vertically_on_load(1);
    const PixelBuffer imgBuffer = (const PixelBuffer)(buffer);
    m_Data = stbi_load_from_memory(imgBuffer, static_cast<int>(numBytes), &m_Width, &m_Height, &m_Channels, STBI_rgb);
    VCT_INFO("JPG Image Data at address: {0}", (void*)m_Data);
}

void Image::LoadHDR(const void* buffer, size_t numBytes)
{
    ASSERT(false, "Not Implemented");
}

void Image::LoadRAW(const void* buffer, size_t numBytes)
{
    m_Data = (uint8_t*)malloc(sizeof(uint8_t) * numBytes);
    m_Channels = 3;
    memcpy(m_Data, buffer, numBytes);
    VCT_INFO("RAW Image Data at address: {0}", (void*)m_Data);
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
