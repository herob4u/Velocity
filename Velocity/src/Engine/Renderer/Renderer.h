#pragma once

#include "RenderCommands.h"

namespace Vct
{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void BeginScene();
        void EndScene();

        void GenerateTexture(uint32_t& texId, const uint8_t* data, uint16_t width, uint16_t height, uint16_t format, uint16_t wrapMode);
        void DeleteTexture(uint32_t& texId);
        void GenerateBuffer();
        void DeleteBuffer();

        static Renderer& Get();
    private:
        void ProcessCmds();
    private:
        RenderCmdQueue m_TextureCmdQueue;
        RenderCmdQueue m_BufferCmdQueue;

        std::thread m_WorkerThread;
        bool m_Finished;
    };
}