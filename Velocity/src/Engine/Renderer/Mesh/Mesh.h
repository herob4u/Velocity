#pragma once
#include "Engine/Renderer/Types.h"

#include "Engine/Core/Core.h"

namespace Vct
{
    class Mesh
    {
    public:
        static void ComputeTangents(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
                                    const glm::vec2& uv1,
                                    const glm::vec2& uv2,
                                    const glm::vec2& uv3,
                                    glm::vec3& outTangent, glm::vec3& outBiTangent);

        Mesh(uint32_t numVertices, uint16_t numTriangles);
        ~Mesh();

        FORCEINLINE uint32_t GetNumVertices() const  { return NumVertices; }
        FORCEINLINE uint16_t GetNumTriangles() const { return NumTriangles; }

        void Draw();
    protected:
        void InitBuffers(const void* vertexData, size_t size, const std::vector<Triangle>& indices);
        void SetBufferLayout(const BufferLayout& layout);
    protected:
        uint32_t NumVertices;
        uint16_t NumTriangles;
    private:
        unsigned int m_vbo, m_ibo, m_vao;
    };

    class SkeletalMesh : public Mesh
    {
    public:
        SkeletalMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<Triangle>& indices);
    };

    class StaticMesh : public Mesh
    {
    public:
        StaticMesh(const std::vector<MeshVertex>& vertices, const std::vector<Triangle>& indices);
    };

    class SkyboxMesh : public Mesh
    {
    public:
        SkyboxMesh(const std::vector<SkyboxVertex>& vertices, const std::vector<Triangle>& indices);
    };

    class ColorMesh : public Mesh
    {
    public:
        ColorMesh(const std::vector<ColoredVertex>& vertices, const std::vector<Triangle>& indices);
    };
}