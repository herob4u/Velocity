#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Types/Path.h"

namespace Vct
{
    class Image;

    class Cubemap
    {
    public:
        Cubemap(const Path& right,
                const Path& left,
                const Path& top,
                const Path& bottom,
                const Path& back,
                const Path& front);

        ~Cubemap();

        void Destroy();

        FORCEINLINE uint32_t GetRendererId() const { return m_RendererId; }
    protected:
        void InitializeCubemap(const std::vector<Image*> faces);
    private:
        uint32_t m_RendererId;
    };
}