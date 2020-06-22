#pragma once

#include "Engine/Resources/Resource.h"

namespace Vct
{
    class ShaderSource;

    static const Resource::Type ResType_Shader("shader");

    class Shader
    {
    public:
        Shader(const StringId shaderId);
        ~Shader();

        void RebuildProgram(const Path& vertexShaderPath, const Path& fragmentShaderPath);

        FORCEINLINE const StringId& GetShaderName() const { return m_ShaderName; }
        bool GetBinary(void* outData, size_t& bytes) const;
    private:

        void CreateShader(class ShaderSource* shaderSrc);
    private:
        StringId m_ShaderName;
        uint32_t m_RendererId;
        TResourcePtr<ShaderSource> m_VertexShader;
        TResourcePtr<ShaderSource> m_FragmentShader;

        bool bIsCompiled;
    };
}