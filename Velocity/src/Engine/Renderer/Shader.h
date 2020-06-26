#pragma once

#include "Engine/Resources/Resource.h"
#include "glm/glm.hpp"

namespace Vct
{
    class ShaderSource;
    class ShaderCache;

    static const Resource::Type ResType_Shader("shader");

    enum ShaderType : int
    {
        VERTEX_SHADER,
        FRAGMENT_SHADER,
        GEOMETRY_SHADER
    };

    class Shader //: public Resource
    {
    public:
        enum Attributes : uint8_t
        {
            VertexPosition      = 1 << 1,
            VertexNormal        = 1 << 2,
            VertexTangent       = 1 << 3,
            VertexBiTangent     = 1 << 4,
            VertexUV            = 1 << 5,
            VertexColor         = 1 << 6,
        };

        Shader(const StringId& shaderName);

        ~Shader();

        void Recompile();
        void SetAttributes(Attributes attr);

        void Bind() const;
        void Unbind() const;

        FORCEINLINE const StringId& GetShaderName() const { return m_ShaderName; }
        FORCEINLINE Attributes GetAttributes() const      { return m_Attributes; }
        bool GetBinary(void** outData, size_t& bytes, int& format) const;

        void SetUniform1f(const StringId& uniform, float x);
        void SetUniformVec2f(const StringId& uniform, const glm::vec2& vec);
        void SetUniformVec3f(const StringId& uniform, const glm::vec3& vec);
        void SetUniformVec4f(const StringId& uniform, const glm::vec4& vec);

        void SetUniform1i(const StringId& uniform, int x);
        void SetUniformVec2i(const StringId& uniform, const glm::ivec2& vec);
        void SetUniformVec3i(const StringId& uniform, const glm::ivec3& vec);
        void SetUniformVec4i(const StringId& uniform, const glm::ivec4& vec);

        void SetUniformMat2(const StringId& uniform, const glm::mat2& mat, bool transpose = false);
        void SetUniformMat3(const StringId& uniform, const glm::mat3& mat, bool transpose = false);
        void SetUniformMat4(const StringId& uniform, const glm::mat4& mat, bool transpose = false);

        void SetUniformArray1i(const StringId& uniform, int* values, uint32_t count);
        void SetUniformArray1f(const StringId& uniform, float* values, uint32_t count);


    protected:
        bool HasLinkErrors(std::vector<char>& outErrorMsg);

        void ScanUniforms();

        // Caches a uniform location with the specified uniformName
        int CacheUniform(const StringId& uniformName);

        // Finds the location of a cached uniform
        int FindUniformLocation(const StringId& uniformName) const;
    private:

    private:
        StringId m_ShaderName;
        uint32_t m_RendererId;
        TResourcePtr<ShaderSource> m_VertexShader;
        TResourcePtr<ShaderSource> m_FragmentShader;

        std::array<StringId, 64> m_UniformCache;

        Attributes m_Attributes;
    };

    class ShaderSource final : public Resource
    {
    public:
        ShaderSource(const Path& shaderPath);
        ShaderSource(const std::string& shaderPath);

        bool IsCompiled() const { return IsLoaded(); }
        uint32_t& RendererId() { return m_RendererId; }
        ShaderType GetShaderType() const { return m_ShaderType; }
        const std::string& GetShaderStr() const { return m_ShaderStr; }

        RES_TYPE(ResType_Shader);
    protected:
        /* Resource Interface */
        virtual bool Load(const void* rawBinary, size_t bytes) override;
        virtual void Unload() override;

        bool InitFromSource(const std::string& shaderSrc);
        bool HasCompileErrors(std::vector<char>& ourErrorMsg);

        ShaderType ParseShaderType(const Path& shaderPath) const;
        ShaderType ParseShaderType(const std::string& shaderPath) const;
        std::string GenerateHeader(Shader::Attributes attr) const;

        uint8_t ScanAttributes(const std::string& shaderSrc) const;
    private:
        uint32_t m_RendererId;
        ShaderType m_ShaderType;
        std::string m_ShaderStr;
    };
}