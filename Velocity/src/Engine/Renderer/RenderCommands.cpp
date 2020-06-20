#include "vctPCH.h"
#include "RenderCommands.h"

/* Experimental */
#include "Texture/Texture2D.h"
#include "Mesh/MeshFactory.h"
#include "Mesh/Mesh.h"
#include "ShaderProgram.h"

#include "glad/glad.h"

using namespace Vct;

void RenderCommands::ClearBuffers()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderCommands::SetClearColor(const glm::vec3 & color)
{
    glClearColor(color.r, color.g, color.b, 1.f);
}

void RenderCommands::SetClearColor(float r, float g, float b)
{
    glClearColor(r, g, b, 1.f);
}

void RenderCommands::DrawImage(Ref<ShaderProgram> shader, Texture2D* texture)
{
    if(!texture)
        VCT_WARN("No background texture found");

    if(texture && shader.get())
    {
        static float vertices[] = {-1.f, -1.f, 0.f,     -1.f, 1.f, 0.f
                                , 1.f, -1.f, 0.f,      1.f, 1.f, 0.f };

        static float uvs[] = { 0.f, 0.f,    0.f, 1.f
                            , 1.f, 0.f,    1.f, 1.f };
        
        static uint32_t indices[] = {0, 3, 1,  0, 2, 3};

        static GLuint vao;
        
        shader->enable();

        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);
            GLuint vertexBuffer;
            GLuint texCoordBuffer;
            GLuint indexBuffer;

            GLuint posAttribute = shader->getAttribLocation("a_Position");
            GLuint texAttribute = shader->getAttribLocation("a_TexCoord");
            glEnableVertexAttribArray(posAttribute);
            glEnableVertexAttribArray(texAttribute);

            glGenBuffers(1, &vertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(posAttribute, 3, GL_FLOAT, false, 0, nullptr);

            glGenBuffers(1, &texCoordBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
            glVertexAttribPointer(texAttribute, 2, GL_FLOAT, false, 0, nullptr);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &indexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            texture->Bind();
            glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(uint32_t), GL_UNSIGNED_INT, 0);
            texture->Unbind();


            glDeleteBuffers(1, &vertexBuffer);
            glDeleteBuffers(1, &texCoordBuffer);
            glDeleteBuffers(1, &indexBuffer);

        glBindVertexArray(0);
        glDeleteBuffers(1, &vao);

        shader->disable();
    }
}

void RenderCommands::SetWireframeMode(bool enabled)
{
    enabled ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderCmdQueue::Enqueue(RenderCmd* cmd)
{

    if(cmd->MustFlush())
    {
        Flush();
    }

    std::unique_lock<std::mutex> queueLock(m_QueueMutex);
    m_CmdQueue.push(cmd);
}

void RenderCmdQueue::Flush()
{
    std::unique_lock<std::mutex> queueLock(m_QueueMutex);
    while(m_CmdQueue.size() > 0)
    {
        RenderCmd* cmd = m_CmdQueue.front();
        m_CmdQueue.pop();

        cmd->Execute();

        delete cmd;
    }
}

void RenderCmdQueue::ProcessNext()
{
    if(m_CmdQueue.size() == 0)
        return;

    std::unique_lock<std::mutex> queueLock(m_QueueMutex);
    if(m_CmdQueue.size() > 0)
    {
        RenderCmd* cmd = m_CmdQueue.front();
        m_CmdQueue.pop();

        queueLock.unlock();

        cmd->Execute();

        delete cmd;
    }
}

void RenderCmdQueue::TryProcessNext()
{
    if(m_CmdQueue.size() == 0)
        return;

    std::unique_lock<std::mutex> queueLock(m_QueueMutex, std::defer_lock);

    if(queueLock.try_lock() && m_CmdQueue.size() > 0)
    {
        RenderCmd* cmd = m_CmdQueue.front();
        m_CmdQueue.pop();
        queueLock.unlock();

        cmd->Execute();

        delete cmd;
    }
}

GenTexturesCmd::GenTexturesCmd(uint32_t& texId, const uint8_t* data, uint16_t width, uint16_t height, uint16_t format, uint16_t type, uint16_t wrapMode)
    : m_TexId(texId)
    , m_Data(data)
    , m_Width(width)
    , m_Height(height)
    , m_Format(format)
    , m_Type(type)
    , m_WrapMode(wrapMode)
{
}

void GenTexturesCmd::Execute()
{
    ASSERT(m_Data, "Invalid texture data, were resources deallocated before a flush?");

    VCT_INFO("Processing {0}", GetDebugName());

    glGenTextures(1, &m_TexId);
    glBindTexture(GL_TEXTURE_2D, m_TexId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, m_Format, m_Width, m_Height, 0, m_Format, m_Type, m_Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

DelTexturesCmd::DelTexturesCmd(uint32_t& texId)
    : m_TexId(texId)
{
}

void DelTexturesCmd::Execute()
{
    VCT_INFO("Processing {0}", GetDebugName());

    glDeleteTextures(1, &m_TexId);
}
