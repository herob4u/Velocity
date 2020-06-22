#pragma once

#include "Engine/Resources/Resource.h"

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

        void Bind();
        void Unbind();

        FORCEINLINE const StringId& GetShaderName() const { return m_ShaderName; }
        FORCEINLINE Attributes GetAttributes() const      { return m_Attributes; }
        bool GetBinary(void** outData, size_t& bytes) const;

    protected:
        bool HasLinkErrors(std::vector<char>& outErrorMsg);
    private:

        void CreateShader(class ShaderSource* shaderSrc);
    private:
        StringId m_ShaderName;
        uint32_t m_RendererId;
        TResourcePtr<ShaderSource> m_VertexShader;
        TResourcePtr<ShaderSource> m_FragmentShader;

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

        RES_TYPE(ResType_Shader);
    protected:
        /* Resource Interface */
        virtual bool Load(const void* rawBinary, size_t bytes) override;
        virtual void Unload() override;

        bool InitFromSource(const std::string& shaderSrc);
        ShaderType ParseShaderType(const Path& shaderPath) const;
        ShaderType ParseShaderType(const std::string& shaderPath) const;
        std::string GenerateHeader(Shader::Attributes attr) const;
        uint8_t ScanAttributes(const std::string& shaderSrc) const;
        bool HasCompileErrors(std::vector<char>& ourErrorMsg);
    private:
        uint32_t m_RendererId;
        ShaderType m_ShaderType;
        std::string m_ShaderStr;
    };
}