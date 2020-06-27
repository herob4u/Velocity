#include "vctPCH.h"
#include "Shader.h"

#include "Engine/Engine.h"
#include "GLError.h"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

// Annoying sprintf/sscanf safety warning
#pragma warning(disable:4996)

using namespace Vct;

static const StringId SHADER_DIRECTORY = SHADER_DIR;
static const StringId VSHADER_EXT = ".vs";
static const StringId FSHADER_EXT = ".fs";
static const StringId GSHADER_EXT = ".gs";

/*===========================================================*/
//                      SHADER SOURCE
/*===========================================================*/

namespace Vct
{ 
    static int GetGLShaderType(ShaderType shaderType)
    {
        switch(shaderType)
        {
            case ShaderType::VERTEX_SHADER: return GL_VERTEX_SHADER;
            case ShaderType::GEOMETRY_SHADER: return GL_GEOMETRY_SHADER;
            case ShaderType::FRAGMENT_SHADER: return GL_FRAGMENT_SHADER;

            default: ASSERT(false, "Invalid shader type"); return ShaderType::VERTEX_SHADER;
        }
    }

    ShaderSource::ShaderSource(const Path& shaderPath)
        : Resource(shaderPath)
    {
        m_ShaderType = ParseShaderType(shaderPath);
    }

    ShaderSource::ShaderSource(const std::string& shaderPath)
        : Resource(shaderPath)
    {
        m_ShaderType = ParseShaderType(shaderPath);
    }

    bool ShaderSource::Load(const void* rawBinary, size_t bytes)
    {
        m_ShaderStr = std::string((char*)rawBinary, bytes);
        return InitFromSource(m_ShaderStr);
    }

    void ShaderSource::Unload()
    {
        m_ShaderStr.clear();

        // This is risky, might be deleting another shader since it is not IDed
        if(m_RendererId != 0)
        {
            glDeleteShader(GetGLShaderType(m_ShaderType));
            m_RendererId = 0;
        }
    }

    bool ShaderSource::InitFromSource(const std::string& shaderSrc)
    {
        int glShaderType = GetGLShaderType(m_ShaderType);

        m_RendererId = glCreateShader(glShaderType);

        /*
        std::string fullShaderSrc;
        if(m_ShaderType == ShaderType::VERTEX_SHADER)
        {
        }
        else if(m_ShaderType == ShaderType::GEOMETRY_SHADER)
        {

        }
        */

        const GLchar* shaderSrcStr = (const GLchar*)shaderSrc.c_str();
        glShaderSource(m_RendererId, 1, &shaderSrcStr, 0);

        glCompileShader(m_RendererId);

        // Check for compilation errors
        std::vector<char> errorMsg;
        if(HasCompileErrors(errorMsg))
        {
            VCT_ERROR("Shader Compilation Error: {0}", errorMsg.data());
            // Free resources
            glDeleteShader(glShaderType);
            m_RendererId = 0;

            return false;
        }

        return true;
    }

    ShaderType ShaderSource::ParseShaderType(const Path& shaderPath) const
    {
        std::string ext(3, 0);
        shaderPath.GetExtension(ext, true);

        StringId extId(ext.c_str());

        if(extId == VSHADER_EXT)
        {
            return ShaderType::VERTEX_SHADER;
        }
        else if(extId == FSHADER_EXT)
        {
            return ShaderType::FRAGMENT_SHADER;
        }
        else if(extId == GSHADER_EXT)
        {
           return  ShaderType::GEOMETRY_SHADER;
        }
        else
        {
            ASSERT(false, "Invalid shader file");
            return ShaderType::VERTEX_SHADER;
        }
    }

    ShaderType ShaderSource::ParseShaderType(const std::string& shaderPath) const
    {
        std::string ext(2,0);
        Path::GetExtension(shaderPath, ext);

        StringId extId(ext.c_str());

        if(extId == VSHADER_EXT)
        {
            return ShaderType::VERTEX_SHADER;
        }
        else if(extId == FSHADER_EXT)
        {
            return ShaderType::FRAGMENT_SHADER;
        }
        else if(extId == GSHADER_EXT)
        {
            return  ShaderType::GEOMETRY_SHADER;
        }
        else
        {
            ASSERT(false, "Invalid shader file");
            return ShaderType::VERTEX_SHADER;
        }
    }

    std::string ShaderSource::GenerateHeader(Shader::Attributes attr) const
    {
        return "";
    }

    uint8_t ShaderSource::ScanAttributes(const std::string& shaderSrc) const
    {
        // Read line by line
        std::istringstream iss(shaderSrc);
        std::string line;
        StringId attributeName;
        // each line, construct istringstream to read formatted data
        while(std::getline(iss, line))
        {
            // Attribute with "in (layout=x) <type>" declaration
            if(line.substr(0, 4) == "#in (")
            {
                // Assumes 1 digit for layout number
                int attrIdx = std::atoi(line.substr(10,1).c_str());
                char attrName[32];
                sscanf(line.c_str(), "a_%s", attrName);

                VCT_INFO("Attribute {0}: {1}", attrIdx, attrName);
            }
            // Attribute with "in <type>" declaration
            else if(line.substr(0, 3) == "in ")
            {
                char attrName[32];
                sscanf(line.c_str(), "a_%s", attrName);

                VCT_INFO("Attribute {0}: {1}", attrName);
            }
        }

        return (Shader::Attributes::VertexColor | Shader::Attributes::VertexNormal);
        
    }

    bool ShaderSource::HasCompileErrors(std::vector<char>& outErrorMsg)
    {
        GLint isCompiled = false;
        glGetShaderiv(m_RendererId, GL_COMPILE_STATUS, &isCompiled);

        if(isCompiled == GL_FALSE)
        {
            GLint length = 0;
            glGetShaderiv(m_RendererId, GL_INFO_LOG_LENGTH, &length);

            outErrorMsg.resize(length);
            glGetShaderInfoLog(m_RendererId, length, &length, &outErrorMsg[0]);
        }

        return (isCompiled == GL_FALSE);
    }
}


/*===========================================================*/
//                      SHADER
/*===========================================================*/

Shader::Shader(const StringId& shaderName)
    : m_ShaderName(shaderName)
{
    bool success = false;

    ShaderCache& shaderCache = gEngine->GetShaderCache();

    // If the shader cache already has precompiled binaries for this shader, simply upload it.
    const ShaderBinary& precompiledShader = shaderCache.GetPrecompiledShader(shaderName);
    if(!precompiledShader.IsEmpty())
    {
        m_RendererId = glCreateProgram();
        glProgramBinary(m_RendererId, (GLenum)36385, precompiledShader.Data, (GLsizei)precompiledShader.Bytes); // temporary hack for format until we read it from file properly

        std::vector<char> errorMsg;
        if(HasLinkErrors(errorMsg))
        {
            VCT_WARN("Shader Link Error, recompiling from source: {0}", errorMsg.data());
            success = false;
        }
        else
        {
            success = true;
        }
    }

    if(!success)
    {
        Recompile();

        std::vector<char> errorMsg;
        if(HasLinkErrors(errorMsg))
        {
            VCT_ERROR("Shader Link Error: {0}", errorMsg.data());
            glDeleteProgram(m_RendererId);

            m_VertexShader = nullptr;
            m_FragmentShader = nullptr;
        }
        else
        {
            GL_CHECK_ERROR();
            shaderCache.CacheProgram(*this);
        }
    }

    GL_CHECK_ERROR();
    ScanUniforms();
}

Shader::~Shader()
{
}

void Shader::Recompile()
{
    char vertexShaderPath[MAX_PATH_LENGTH];
    char fragmentShaderPath[MAX_PATH_LENGTH];

    // This is wrong, we want relative paths when initializing resources! FileMgr handles the abs path
    //sprintf(vertexShaderPath,   "%s%s%s", SHADER_DIR, shaderName.ToStringRef(), ".vs");
    //sprintf(fragmentShaderPath, "%s%s%s", SHADER_DIR, shaderName.ToStringRef(), ".fs");

    sprintf(vertexShaderPath, "%s%s%s", "Shaders/", m_ShaderName.ToStringRef(), ".vs");
    sprintf(fragmentShaderPath, "%s%s%s", "Shaders/", m_ShaderName.ToStringRef(), ".fs");

    m_VertexShader = Path(vertexShaderPath);
    m_FragmentShader = Path(fragmentShaderPath);

    // Sync or Async load?
    m_VertexShader.Load();
    m_FragmentShader.Load();

    // Both shaders loaded - compile and link them ...

    ASSERT(m_VertexShader.IsValid() && m_FragmentShader.IsValid(), "Shader Program incomplete");

    m_RendererId = glCreateProgram();
    glAttachShader(m_RendererId, m_VertexShader.Get()->RendererId());
    glAttachShader(m_RendererId, m_FragmentShader.Get()->RendererId());

    glLinkProgram(m_RendererId);
    
}

void Shader::SetAttributes(Attributes attr)
{
    m_Attributes = attr;
}

void Shader::Bind() const
{
    GL_CHECK_ERROR();

    glUseProgram(m_RendererId);

    GL_CHECK_ERROR();
}

void Shader::Unbind() const
{
    GL_CHECK_ERROR();

    glUseProgram(0);

    GL_CHECK_ERROR();
}

bool Shader::GetBinary(void** outData, size_t& bytes, int& format) const
{
    if(m_RendererId != 0)
    {
        GLint length = 0;
        glGetProgramiv(m_RendererId, GL_PROGRAM_BINARY_LENGTH, &length);

        ASSERT(length > 0, "Unexpected length for compiled shader binary");

        bytes       = length;
        *outData    = malloc(bytes);

        glGetProgramBinary(m_RendererId, length, NULL, (GLenum*)&format, *outData);

        return true;
    }

    return false;
}

bool Shader::HasLinkErrors(std::vector<char>& outErrorMsg)
{
    GLint isLinked = 0;
    //glGetProgramiv(m_RendererId, GL_LINK_STATUS, (int *)&isLinked);
    glGetProgramiv(m_RendererId, GL_LINK_STATUS, &isLinked);
    if(isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(m_RendererId, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        outErrorMsg.resize(maxLength);
        glGetProgramInfoLog(m_RendererId, maxLength, &maxLength, &outErrorMsg[0]);
    }

    return (isLinked == GL_FALSE);
}

void Shader::ScanUniforms()
{
    int numUniforms = 0;
    glGetProgramiv(m_RendererId, GL_ACTIVE_UNIFORMS, &numUniforms);

    for(int i = 0; i < numUniforms; i++)
    {
        GLchar uniformName[32];
        GLenum uniformType;
        GLint uniformSize;

        glGetActiveUniform(m_RendererId, i, 32, nullptr, &uniformSize, &uniformType, uniformName);
        
        GLint location = glGetUniformLocation(m_RendererId, uniformName);
        //ASSERT(location != -1, "Expected valid uniform location. Was uniform name invalid?");
        if(location != -1)
        m_UniformCache[location] = StringId(uniformName);

        VCT_INFO("Uniform: {0} {1}, size={2}", uniformType, uniformName, uniformSize);
    }
}

int Shader::CacheUniform(const StringId& uniformName)
{
    int location = glGetUniformLocation(m_RendererId, uniformName.ToStringRef());
    ASSERT(location != -1, "Uniform '{0}' does not exist", uniformName.ToStringRef());
    ASSERT(location < 64, "Uniform location index out of range");
    m_UniformCache[location] = uniformName;
    return location;
}

int Shader::FindUniformLocation(const StringId & uniformName) const
{
    for(int i = 0; i < 64; i++)
    {
        if(m_UniformCache[i] == uniformName)
            return i;
    }

    return -1;
}

// Uniforms 

void Shader::SetUniform1f(const StringId& uniform, float x)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform1f(location, x);
}

void Shader::SetUniformVec2f(const StringId& uniform, const glm::vec2& vec)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform2f(location, vec.x, vec.y);
}

void Shader::SetUniformVec3f(const StringId& uniform, const glm::vec3& vec)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::SetUniformVec4f(const StringId& uniform, const glm::vec4& vec)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetUniform1i(const StringId& uniform, int x)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform1i(location, x);
}

void Shader::SetUniformVec2i(const StringId& uniform, const glm::ivec2& vec)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform2i(location, vec.x, vec.y);
}

void Shader::SetUniformVec3i(const StringId& uniform, const glm::ivec3& vec)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform3i(location, vec.x, vec.y, vec.z);
}

void Shader::SetUniformVec4i(const StringId& uniform, const glm::ivec4& vec)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform4i(location, vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetUniformMat2(const StringId& uniform, const glm::mat2& mat, bool transpose)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniformMatrix2fv(location, 1, transpose, glm::value_ptr(mat));
}

void Shader::SetUniformMat3(const StringId& uniform, const glm::mat3& mat, bool transpose)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniformMatrix3fv(location, 1, transpose, glm::value_ptr(mat));
}

void Shader::SetUniformMat4(const StringId& uniform, const glm::mat4& mat, bool transpose)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(mat));
}

void Shader::SetUniformArray1i(const StringId& uniform, int* values, uint32_t count)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform1iv(location, count, values);
}

void Shader::SetUniformArray1f(const StringId& uniform, float* values, uint32_t count)
{
    GL_CHECK_ERROR();

    int location = FindUniformLocation(uniform);

    if(location == -1)
    {
        location = CacheUniform(uniform);
    }

    glUniform1fv(location, count, values);
}