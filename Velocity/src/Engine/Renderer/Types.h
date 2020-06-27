#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"

#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

namespace Vct
{
    enum class ShaderDataType : uint8_t
    {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };
    
    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch(type)
        {
        case ShaderDataType::Float:         return 4;
        case ShaderDataType::Float2:        return 4 * 2;
        case ShaderDataType::Float3:        return 4 * 3;
        case ShaderDataType::Float4:        return 4 * 4;

        case ShaderDataType::Mat3:          return 4 * 3 * 3;
        case ShaderDataType::Mat4:          return 4 * 4 * 4;

        case ShaderDataType::Int:           return 4;
        case ShaderDataType::Int2:          return 4 * 2;
        case ShaderDataType::Int3:          return 4 * 3;
        case ShaderDataType::Int4:          return 4 * 4;

        case ShaderDataType::Bool:          return 1;
        }

        ASSERT(false, "Unknown Shader Data Type");
        return 0;
    }

    struct BufferElement
    {
        std::string Name;
        ShaderDataType Type;
        uint32_t Offset; // Dependent on the layout of other elements
        uint32_t Size;
        bool Normalized;

        BufferElement()
        {
        }

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
        {
        }

        // Returns the number of components a data type contains. For example, a Float3 is composed of 3 numbers/components.
        // An Int4 is composed of 4 numbers/components.
        uint32_t GetElementCount() const
        {
            switch(Type)
            {
            case ShaderDataType::Float:         return 1;
            case ShaderDataType::Float2:        return 2;
            case ShaderDataType::Float3:        return 3;
            case ShaderDataType::Float4:        return 4;

            case ShaderDataType::Mat3:          return 3 * 3;
            case ShaderDataType::Mat4:          return 4 * 4;

            case ShaderDataType::Int:           return 1;
            case ShaderDataType::Int2:          return 2;
            case ShaderDataType::Int3:          return 3;
            case ShaderDataType::Int4:          return 4;

            case ShaderDataType::Bool:          return 1;
            }

            ASSERT(false, "Unknown Shader Data Type");
            return 0;
        }
    };

    /* Usage:
    *
    *  BufferLayout layout = { {ShaderDataType::Float, "a_Value"}, {ShaderDataType::Float3, "a_Color"} }
    */
    class BufferLayout
    {
    public:
        BufferLayout()
        {
        }

        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : m_Elements(elements) // Vector has a constructor that takes in an initializer list! ez
            , m_Stride(0)
        {
            CalcOffsetsAndStride();
        }

        inline uint32_t GetStride() const { return m_Stride; }

        inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.cbegin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.cend(); }

    private:
        void CalcOffsetsAndStride()
        {
            uint32_t offset = 0;
            m_Stride = 0;

            for(auto& element : m_Elements)
            {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride;
    };

/* ================================================================
*                           VERTEX TYPES
* ===============================================================*/
    enum class VertexType : short
    {
        VertexPNCUV = 0,
        VertexPNUV,
        VertexPUVW,
        VertexPNUVS
    };

    #define VERTEX_TYPE(type) static VertexType GetType() { return type; }

    typedef struct VertexPNCUV
    {
        VERTEX_TYPE(VertexType::VertexPNCUV)
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 Color;
        glm::vec2 UV;

        static BufferLayout& GetLayout()
        {
            static BufferLayout layout {{ ShaderDataType::Float3, "a_Position" }, 
                                        { ShaderDataType::Float3, "a_Normal" }, 
                                        { ShaderDataType::Float3, "a_Color" }, 
                                        { ShaderDataType::Float2, "a_UV" }};
            return layout;
        }
    } ColoredVertex;

    typedef struct VertexPNUV
    {
        VERTEX_TYPE(VertexType::VertexPNUV)
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 UV;
        glm::vec3 Tangent;
        glm::vec3 BiTangent;

        static BufferLayout& GetLayout()
        {
            static BufferLayout layout {{ ShaderDataType::Float3, "a_Position" },
                                        { ShaderDataType::Float3, "a_Normal" },
                                        { ShaderDataType::Float2, "a_UV" },
                                        { ShaderDataType::Float3, "a_Tangent"},
                                        { ShaderDataType::Float3, "a_BiTangent"} };

            return layout;
        }
    } MeshVertex;

    typedef struct VertexPUVW
    {
        VERTEX_TYPE(VertexType::VertexPUVW)
        glm::vec3 Position;
        glm::vec3 UVW;

        static BufferLayout& GetLayout()
        {
            static BufferLayout layout {{ ShaderDataType::Float3, "a_Position" },
                                        { ShaderDataType::Float3, "a_UVW" } };

            return layout;
        }
    } SkyboxVertex;

    typedef struct VertexPNUVS
    {
        VERTEX_TYPE(VertexType::VertexPNUVS)
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 UV;
        glm::vec3 Tangent;
        glm::vec3 BiTangent;
        glm::ivec4 BoneIndices;
        glm::vec4 BoneWeights;

        static BufferLayout& GetLayout()
        {
            static BufferLayout layout {{ ShaderDataType::Float3, "a_Position" },
                                        { ShaderDataType::Float3, "a_Normal" },
                                        { ShaderDataType::Float2, "a_UV" },
                                        { ShaderDataType::Float3, "a_Tangent" },
                                        { ShaderDataType::Float3, "a_BiTangent" },
                                        { ShaderDataType::Int4, "a_BoneIndices"},
                                        { ShaderDataType::Float4, "a_BoneWeights"} };

            return layout;
        }
    } SkinnedVertex;

    /* ================================================================
    *                           TRIANGLES
    * ===============================================================*/
    struct Triangle
    {
        uint32_t Idx[3];

        Triangle(unsigned short v1, unsigned short v2, unsigned short v3)
        {
            Idx[0] = v1;
            Idx[1] = v2;
            Idx[2] = v3;
        }

        Triangle(const Triangle& Other)
        {
            Idx[0] = Other.Idx[0];
            Idx[1] = Other.Idx[1];
            Idx[2] = Other.Idx[2];

        }
    };

    struct TriangleList
    {
        unsigned short Offset = 0;
        unsigned short Count = 0;
    };
}