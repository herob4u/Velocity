#pragma once

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

#include <queue>

class ShaderProgram;

namespace Vct
{
    class Texture;

    class RenderCommands
    {
    public:
        static void ClearBuffers();
        static void SetClearColor(const glm::vec3& color);
        static void SetClearColor(float r, float g, float b);

        static void DrawImage(Ref<ShaderProgram> shader, Texture* texture);
        static void SetWireframeMode(bool enabled);
    };

    /*  Base interface for all render commands to be enqueued in a Command Queue */
    /*  Some commands such as deletions can only be guaranteed to behave correctly after
    *   previous code has finished executing. For example, we can attempt to allocate a
    *   texture from some buffer. As the command is waiting to be dequeued and processed,
    *   the source buffer may be already destroyed. To avoid that, some commands indicate they
    *   must force a flush, this in effect creates a memory fence to synchronize memory.
    */
    struct RenderCmd
    {
        virtual ~RenderCmd() {}

        virtual void Execute() = 0;
        virtual bool MustFlush() { return false; }
        virtual std::string GetDebugName() const { return std::string("Render Cmd"); }
    };

    class RenderCmdQueue
    {
    public:
        void Enqueue(RenderCmd* cmd);
        void Flush();
        void ProcessNext();
        void TryProcessNext();
    private:
        std::mutex m_QueueMutex;
        std::queue<RenderCmd*> m_CmdQueue;
    };

    struct GenTexturesCmd final : public RenderCmd
    {
    public:
        GenTexturesCmd(uint32_t& texId, const uint8_t* data, uint16_t width, uint16_t height, uint16_t format, uint16_t type, uint16_t wrapMode);
        virtual void Execute() override;
        virtual std::string GetDebugName() const { return std::string("GenTextureCmd"); }

    private:
        uint32_t& m_TexId;
        const uint8_t* m_Data;
        uint16_t m_Width;
        uint16_t m_Height;
        uint16_t m_Format;
        uint16_t m_Type;
        uint16_t m_WrapMode;
    };

    struct DelTexturesCmd final : public RenderCmd
    {
    public:
        DelTexturesCmd(uint32_t& texId);
        virtual void Execute() override;
        virtual bool MustFlush() override { return true; }
        virtual std::string GetDebugName() const { return std::string("DelTexturesCmd"); }

    private:
        uint32_t m_TexId;
    };

    struct GenBuffersCmd final : public RenderCmd
    {
    public:
        virtual void Execute() override;
    };

    struct DelBuffersCmd final : public RenderCmd
    {
    public:
        virtual void Execute() override;
    };
}
