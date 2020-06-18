#pragma once
#include "Engine/Core/Types/Path.h"

typedef uint8_t* PixelBuffer;

namespace Vct
{
    enum class ImageFormat : uint8_t
    {
        PNG,
        TGA,
        BMP,
        JPG,
        HDR,
        RAW,
        UNKNOWN
    };

    /* Raw byte-sized image format */
    class Image
    {
    public:
        static ImageFormat GetImageFormat(const std::string& filepath);
        static ImageFormat GetImageFormat(const Path& filepath);

        /* Acquires ownership of an image represented by memory located in sourceBuffer */
        static Image* Acquire(int width, int height, void* sourceBuffer);

        Image(const std::string& filePath);
        Image(const Path& filePath);
        Image(const void* sourceBuffer, size_t numBytes, ImageFormat asFormat);
        ~Image();

        FORCEINLINE int GetWidth() const { return m_Width; }
        FORCEINLINE int GetHeight() const { return m_Height; }
        FORCEINLINE int GetNumChannels() const { return m_Channels; }
        FORCEINLINE int GetDepth() const { return m_Depth; } // aka BPP
        FORCEINLINE const PixelBuffer GetData() const { return m_Data; }
        FORCEINLINE ImageFormat GetFormat() const { return m_Format; }

        void Write(const std::string& filepath) const;

    protected:
        Image();

        void LoadTGA(const void* buffer, size_t numBytes);
        void LoadPNG(const void* buffer, size_t numBytes);
        void LoadBMP(const void* buffer, size_t numBytes);
        void LoadJPG(const void* buffer, size_t numBytes);
        void LoadHDR(const void* buffer, size_t numBytes);
        void LoadRAW(const void* buffer, size_t numBytes);

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
}