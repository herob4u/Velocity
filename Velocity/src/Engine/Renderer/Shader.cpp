#include "vctPCH.h"
#include "Shader.h"

#include "glad/glad.h"

using namespace Vct;

/*===========================================================*/
//                      SHADER SOURCE
/*===========================================================*/

namespace Vct
{ 
    class ShaderSource : public Resource
    {
    public:
        uint32_t& RendererId() { return m_RendererId; }

        RES_TYPE(ResType_Shader);
    protected:
        /* Resource Interface */
        virtual bool Load(const void* rawBinary, size_t bytes) override;
        virtual void Unload() override;
    private:
        uint32_t m_RendererId;
        std::string m_ShaderStr;
    };


    bool ShaderSource::Load(const void* rawBinary, size_t bytes)
    {
        m_ShaderStr = std::string((char*)rawBinary, bytes);
        return true;
    }

    void ShaderSource::Unload()
    {
        m_ShaderStr.clear();
    }
}


/*===========================================================*/
//                      SHADER
/*===========================================================*/
Shader::Shader(const StringId shaderId)
{

}

Shader::~Shader()
{
}

void Shader::RebuildProgram(const Path& vertexShaderPath, const Path& fragmentShaderPath)
{
    m_VertexShader = vertexShaderPath;
    m_FragmentShader = fragmentShaderPath;

    m_VertexShader.Load();
    m_FragmentShader.Load();

    ASSERT(m_VertexShader.IsValid() && m_FragmentShader.IsValid(), "Shader Program incomplete");

    
}

bool Shader::GetBinary(void* outData, size_t& bytes) const
{
    if(m_RendererId != 0)
    {
        GLint length = 0;
        glGetProgramiv(m_RendererId, GL_PROGRAM_BINARY_LENGTH, &length);

        ASSERT(length > 0, "Unexpected length for compiled shader binary");

        bytes       = length;
        outData     = malloc(bytes);

        GLenum format = 0;
        glGetProgramBinary(m_RendererId, length, NULL, &format, outData);

        return true;
    }

    return false;
}

void Shader::CreateShader(ShaderSource* shaderSrc)
{
    if(!shaderSrc)
        return;
}
