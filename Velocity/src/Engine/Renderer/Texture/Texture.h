#pragma once

namespace Vct
{
    class Texture
    {
    public:
        Texture()
            : RendererId(0)
        {
        }

        virtual void Destroy() = 0;

        FORCEINLINE uint32_t GetRendererId() const { return RendererId; }
    protected:
        uint32_t RendererId;
    };
}