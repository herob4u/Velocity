#pragma once

#include "RenderCommands.h"
#include "Buffers.h"
#include "Camera.h"

namespace Vct
{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void BeginScene(const Camera& camera);
        void EndScene();

        void GenerateTextureAsync(uint32_t& texId, const uint8_t* data, uint16_t width, uint16_t height, uint16_t format, uint16_t type, uint16_t wrapMode);
        void DeleteTextureAsync(uint32_t& texId);

        static void GenerateTexture(uint32_t& texId, const uint8_t* data, uint16_t width, uint16_t height, uint16_t format, uint16_t type, uint16_t wrapMode);
        static void DeleteTexture(Texture2D& texture);
        static void GenerateFramebuffer(Framebuffer& fb);
        static void DeleteFramebuffer(Framebuffer& fb);
        static void Bind(Framebuffer& fb);
        static void Unbind(Framebuffer&);

        static Renderer& Get();
    private:
        // Executed on worker thread. Currently unused because we want all async calls to be deferred commands that are flushed before
        // the scene begins rendering. It is exclusively used for resource loading so far.
        void ProcessCmds();
    private:
        RenderCmdQueue m_TextureCmdQueue;
        RenderCmdQueue m_BufferCmdQueue;

        std::thread m_WorkerThread;
        bool m_Finished;
    };
}