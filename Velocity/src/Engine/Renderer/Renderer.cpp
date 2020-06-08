#include "vctPCH.h"

#include "Renderer.h"

using namespace Vct;

Renderer::Renderer()
    : m_Finished(false)
{
    //m_WorkerThread = std::thread(&Renderer::ProcessCmds, this);
}

Renderer::~Renderer()
{
    //m_Finished = true;
    //m_WorkerThread.join();
}

void Renderer::BeginScene()
{
    m_TextureCmdQueue.Flush();
    m_BufferCmdQueue.Flush();
}

void Renderer::EndScene()
{
}

void Renderer::GenerateTexture(uint32_t& texId, const uint8_t* data, uint16_t width, uint16_t height, uint16_t format, uint16_t wrapMode)
{
   RenderCmd* cmd = new GenTexturesCmd(texId, data, width, height, format, wrapMode);
   m_TextureCmdQueue.Enqueue(cmd);
}

void Renderer::DeleteTexture(uint32_t & texId)
{
    RenderCmd* cmd = new DelTexturesCmd(texId);
    m_TextureCmdQueue.Enqueue(cmd);
}

Renderer& Renderer::Get()
{
    static Renderer self;
    return self;
}

void Renderer::ProcessCmds()
{
    while(!m_Finished)
    {
        m_TextureCmdQueue.TryProcessNext();
        m_BufferCmdQueue.TryProcessNext();
    }
}
