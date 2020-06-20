#pragma once

#include "Texture.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Types/Path.h"

namespace Vct
{
    class Image;

    class Cubemap : public Texture
    {
    public:
        Cubemap(const Path& right,
                const Path& left,
                const Path& top,
                const Path& bottom,
                const Path& back,
                const Path& front);

        ~Cubemap();

        virtual void Destroy() override;
    protected:
        void InitializeCubemap(const std::vector<Image*> faces);
    private:
    };
}