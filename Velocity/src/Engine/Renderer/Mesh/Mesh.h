#pragma once
#include "Engine/Renderer/Types.h"

#include "Engine/Core/Core.h"

namespace Vct
{
    class Material;

    class Mesh
    {
    public:
        Mesh(uint32_t numVertices, uint16_t numTriangles);
        //Mesh(const std::vector<MeshVertex>& vertices, const std::vector<Triangle>& indices);
        //Mesh(const std::vector<SkinnedVertex>& vertices, const std::vector<Triangle>& indices);

        FORCEINLINE uint32_t GetNumVertices() const  { return NumVertices; }
        FORCEINLINE uint16_t GetNumTriangles() const { return NumTriangles; }
    protected:
        void InitBuffers(const void* vertexData, size_t size, const std::vector<Triangle>& indices);
        void SetBufferLayout(const BufferLayout& layout);
    protected:
        uint32_t NumVertices;
        uint16_t NumTriangles;
    private:
        /* Triangle list mapping a Material Id to a range of triangles */
        std::vector<TriangleList> m_TriList;

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
}