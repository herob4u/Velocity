#pragma once

#include "RenderCommands.h"
#include "Buffers.h"
#include "Camera.h"

namespace Vct
{
    class Mesh;
    class MaterialInstance;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Init();
        void Shutdown();

        void BeginScene(const Camera& camera);
        void Submit(Mesh* mesh, const glm::mat4& transform, MaterialInstance* matInstance);
        void EndScene();
        
        void RenderCubemap();

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
        // Register all resource mgrs associated with rendering resources.
        void RegisterMgrs();

        // Executed on worker thread. Currently unused because we want all async calls to be deferred commands that are flushed before
        // the scene begins rendering. It is exclusively used for resource loading so far.
        void ProcessCmds();

        void Draw();
    private:
        RenderCmdQueue m_TextureCmdQueue;
        RenderCmdQueue m_BufferCmdQueue;

        std::unique_ptr<Framebuffer> m_CubemapBuffer;

        std::thread m_WorkerThread;
        bool m_Finished;

        struct RenderItem
        {
            Mesh* MeshItem;
            glm::mat4 Transform;
            MaterialInstance* MatInstance;

            RenderItem(Mesh* mesh, const glm::mat4& transform, MaterialInstance* matInstance)
                : MeshItem(mesh)
                , Transform(transform)
                , MatInstance(matInstance)
            {
            }
        };

        struct RenderData
        {
            glm::mat4 View;
            glm::mat4 Projection;
            
            std::vector<RenderItem> RenderItems;
        };

        RenderData m_RenderData;
    };
}