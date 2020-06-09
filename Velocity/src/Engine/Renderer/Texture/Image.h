#pragma once
#include "Engine/Core/Types/Path.h"

typedef uint8_t* PixelBuffer;

enum class ImageFormat : uint8_t
{
    PNG,
    TGA,
    BMP,
    JPG,
    UNKNOWN
};

/* Raw byte-sized image format */
class Image
{
public:
    static ImageFormat GetImageFormat(const std::string& filepath);
    static ImageFormat GetImageFormat(const Path& filepath);

    Image(const std::string& filePath);
    Image(const Path& filePath);
    Image(const void* const sourceBuffer, size_t numBytes, ImageFormat asFormat);
    ~Image();

    FORCEINLINE int GetWidth() const { return m_Width; }
    FORCEINLINE int GetHeight() const { return m_Height; }
    FORCEINLINE int GetNumChannels() const { return m_Channels; }
    FORCEINLINE int GetDepth() const { return m_Depth; } // aka BPP
    FORCEINLINE const PixelBuffer GetData() const { return m_Data; }
    FORCEINLINE ImageFormat GetFormat() const { return m_Format; }

protected:
    void LoadTGA(const void* const buffer, size_t numBytes);
    void LoadPNG(const void* const buffer, size_t numBytes);
    void LoadBMP(const void* const buffer, size_t numBytes);
    void LoadJPG(const void* const buffer, size_t numBytes);

private:
    void Release();
    void CalculateDepth();

    PixelBuffer m_Data;
    int m_Width;
    int m_Height;
    int m_Channels;
    int m_Depth;
    ImageFormat m_Format;
};