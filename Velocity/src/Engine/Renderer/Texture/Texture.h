#pragma once

namespace Vct
{
    class Texture
    {
    public:
        enum class Format : uint8_t
        {
            GREYSCALE,
            RG,
            RGB,
            RGBA,
            FLOAT,
            DEPTH_STENCIL
        };

        enum class DataType : uint8_t
        {
            UNSIGNED_BYTE,
            FLOAT,
            INT,
            DEPTH_STENCIL
        };

        enum WrapMode : uint8_t
        {
            REPEAT,
            CLAMP,
            MIRROR
        };

        struct Params
        {
            uint16_t Width = 256;
            uint16_t Height = 256;
            Format Format = Format::RGBA;
            DataType DataType = DataType::UNSIGNED_BYTE;
            WrapMode WrapMode = WrapMode::REPEAT;

            Params()
            {
            }

            Params(uint16_t width, uint16_t height, Texture::Format format, Texture::DataType dataType, Texture::WrapMode wrapMode)
                : Width(width)
                , Height(height)
                , Format(format)
                , DataType(dataType)
                , WrapMode(wrapMode)
            {
            }
        };

        Texture()
            : RendererId(0)
        {
        }

        virtual ~Texture() {}
        virtual void Destroy() = 0;

        FORCEINLINE uint32_t GetRendererId() const { return RendererId; }
    protected:
        uint32_t RendererId;
    };
}