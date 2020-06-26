#pragma once

#include "Texture.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Types/Path.h"

namespace Vct
{
    class Image;

    enum CubemapFace : uint_fast8_t
    {
        RIGHT = 0,
        LEFT,
        TOP,
        BOTTOM,
        FRONT,
        BACK,
        MAX_VAL
    };

    class Cubemap : public Texture
    {
    public:
        static Cubemap* Allocate(uint16_t width, uint16_t height, Format format, DataType dataType, WrapMode wrapMode = WrapMode::CLAMP);

        Cubemap(const Path& right,
                const Path& left,
                const Path& top,
                const Path& bottom,
                const Path& back,
                const Path& front);

        ~Cubemap();

        std::vector<Ref<Image>> RenderToImages() const;

        virtual void Bind(uint32_t textureSlot) override;
        virtual void Unbind(uint32_t textureSlot) override;
        virtual void Destroy() override;
    protected:
        Cubemap() {}
        void InitializeCubemap(const std::vector<Image*> faces);
    private:

        Params m_Params[CubemapFace::MAX_VAL];
    };
}