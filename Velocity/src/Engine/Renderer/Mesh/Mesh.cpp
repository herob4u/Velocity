#include "vctPCH.h"
#include "Mesh.h"

#include "Engine/Core/Core.h"

#include "glad/glad.h"

using namespace Vct;

static GLenum GLType(ShaderDataType type)
{
    switch(type)
    {
        case ShaderDataType::None:      return GL_INT;
        case ShaderDataType::Float:     return GL_FLOAT;
        case ShaderDataType::Float2:    return GL_FLOAT;
        case ShaderDataType::Float3:    return GL_FLOAT;
        case ShaderDataType::Float4:    return GL_FLOAT;
        case ShaderDataType::Mat3:      return GL_FLOAT;
        case ShaderDataType::Mat4:      return GL_FLOAT;
        case ShaderDataType::Int:       return GL_INT;
        case ShaderDataType::Int2:      return GL_INT;
        case ShaderDataType::Int3:      return GL_INT;
        case ShaderDataType::Int4:      return GL_INT;
        case ShaderDataType::Bool:      return GL_BOOL;

        default:
        {
            ASSERT(false, "Invalid shader type");
            return GL_INT;
        }
        break;  
    }
}

Mesh::Mesh(uint32_t numVertices, uint16_t numTriangles)
    : NumVertices(numVertices)
    , NumTriangles(numTriangles)
{
}

void Mesh::InitBuffers(const void* vertexData, size_t size, const std::vector<Triangle>& triangles)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Interleaved Vertex Buffer
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertexData, GL_STATIC_DRAW);

    // Index Buffer
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * triangles.size(), triangles.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::SetBufferLayout(const BufferLayout& layout)
{
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    uint32_t stride = layout.GetStride();
    int i = 0;
    for(const BufferElement& element : layout)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i++, element.GetElementCount(), GLType(element.Type), element.Normalized, stride, nullptr);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

SkeletalMesh::SkeletalMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<Triangle>& indices)
    : Mesh(vertices.size(), indices.size())
{
    InitBuffers(vertices.data(), sizeof(SkinnedVertex) * vertices.size(), indices);

    BufferLayout& bufferLayout = SkinnedVertex::GetLayout();
    SetBufferLayout(bufferLayout);
}

StaticMesh::StaticMesh(const std::vector<MeshVertex>& vertices, const std::vector<Triangle>& indices)
    : Mesh(vertices.size(), indices.size())
{
    InitBuffers(vertices.data(), sizeof(MeshVertex) * vertices.size(), indices);

    BufferLayout& bufferLayout = MeshVertex::GetLayout();
    SetBufferLayout(bufferLayout);
}

SkyboxMesh::SkyboxMesh(const std::vector<SkyboxVertex>& vertices, const std::vector<Triangle>& indices)
    : Mesh(vertices.size(), indices.size())
{
    InitBuffers(vertices.data(), sizeof(SkyboxVertex) * vertices.size(), indices);

    BufferLayout& bufferLayout = SkyboxVertex::GetLayout();
    SetBufferLayout(bufferLayout);
}
