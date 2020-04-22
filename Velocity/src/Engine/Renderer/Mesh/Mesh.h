#pragma once
#include "Engine/Renderer/Types.h"

namespace Vct
{
    class Material;

    class Mesh
    {
    public:
        Mesh(const std::vector<MeshVertex>& vertices, const std::vector<Triangle>& indices);
        Mesh(const std::vector<SkinnedVertex>& vertices, const std::vector<Triangle>& indices);
    private:
        void GenerateTriangleList(const std::vector<Triangle>& tris);
        void SetBufferLayout(const BufferLayout& layout);
        void InitBuffers(const std::vector<MeshVertex>& vertices, const std::vector<Triangle>& triangles);
    private:
        /* Triangle list mapping a Material Id to a range of triangles */
        std::vector<TriangleList> m_TriList;

        unsigned int m_vbo, m_ibo, m_vao;
    };
}