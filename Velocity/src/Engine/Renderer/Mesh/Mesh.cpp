#include "vctPCH.h"
#include "Mesh.h"

#include "Engine/Core/Core.h"
#include "Engine/Renderer/GLError.h"

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

void Mesh::ComputeTangents( const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, 
                            const glm::vec2& uv1, 
                            const glm::vec2& uv2, 
                            const glm::vec2& uv3,
                            glm::vec3& outTangent, glm::vec3& outBiTangent)
{
    const glm::vec3 e1 = p2 - p1;
    const glm::vec3 e2 = p3 - p2;

    const float du1 = uv2.x - uv1.x;
    const float dv1 = uv2.y - uv1.y;
    const float du2 = uv3.x - uv1.x;
    const float dv2 = uv3.y - uv1.y;

    const float det_inv = 1/(du1*dv2 - du2*dv1);

    outTangent.x = det_inv * (dv2 * e1.x - dv1 * e2.x);
    outTangent.y = det_inv * (dv2 * e1.y - dv1 * e2.y);
    outTangent.z = det_inv * (dv2 * e1.z - dv1 * e2.z);

    outBiTangent.x = det_inv * (-du2 * e1.x + du1* e2.x);
    outBiTangent.y = det_inv * (-du2 * e1.y + du1* e2.y);
    outBiTangent.z = det_inv * (-du2 * e1.z + du1* e2.z);
}

Mesh::Mesh(uint32_t numVertices, uint16_t numTriangles)
    : NumVertices(numVertices)
    , NumTriangles(numTriangles)
{
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
}

void Mesh::Draw()
{
    glBindVertexArray(m_vao);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    GL_CHECK_ERROR();

    glDrawElements(GL_TRIANGLES, GetNumTriangles() * 3, GL_UNSIGNED_INT, nullptr);
    GL_CHECK_ERROR();

    glBindVertexArray(0);
}

void Mesh::InitBuffers(const void* vertexData, size_t size, const std::vector<Triangle>& triangles)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GL_CHECK_ERROR();

    // Interleaved Vertex Buffer
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertexData, GL_STATIC_DRAW);

    GL_CHECK_ERROR();

    // Index Buffer
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * triangles.size(), triangles.data(), GL_STATIC_DRAW);
    GL_CHECK_ERROR();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GL_CHECK_ERROR();
}

void Mesh::SetBufferLayout(const BufferLayout& layout)
{
    GL_CHECK_ERROR();

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    GL_CHECK_ERROR();

    uint32_t stride = layout.GetStride();
    int i = 0;
    for(const BufferElement& element : layout)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i++, element.GetElementCount(), GLType(element.Type), element.Normalized, stride, (void*)(element.Offset));
    }

    GL_CHECK_ERROR();

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

ColorMesh::ColorMesh(const std::vector<ColoredVertex>& vertices, const std::vector<Triangle>& indices)
    : Mesh(vertices.size(), indices.size())
{
    InitBuffers(vertices.data(), sizeof(ColoredVertex) * vertices.size(), indices);

    BufferLayout& bufferLayout = ColoredVertex::GetLayout();
    SetBufferLayout(bufferLayout);
}
