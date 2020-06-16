/*
 * ShaderProgram
 */

#pragma once
#include "Engine/Resources/Resource.h"
#include <string>

static Resource::Type ResType_Shader("shader");

class ShaderProgram : public Resource {
public:
    static std::string getShaderFile(const char* file);

    ShaderProgram();

    ~ShaderProgram();

    void generateProgramObject();

    void attachVertexShader(const char * filePath);
    
    void attachFragmentShader(const char * filePath);
    
    void attachGeometryShader(const char * filePath);

    void link();

    void enable() const;

    void disable() const;

    void recompileShaders();

    unsigned int getProgramObject() const;

    int getUniformLocation(const char* uniformName) const;

    int getAttribLocation(const char* attributeName) const;

    RES_TYPE(ResType_Shader);
protected:
    /* Resource Interface */
    virtual bool Load(const void* rawBinary, size_t bytes) override;
    virtual void Unload() override;

private:
    struct Shader {
        unsigned int shaderObject;
        std::string filePath;

        Shader();
    };

    Shader vertexShader;
    Shader fragmentShader;
    Shader geometryShader;

    unsigned int programObject;
    unsigned int prevProgramObject;
    unsigned int activeProgram;

    void extractSourceCode(std::string & shaderSource, const std::string & filePath);
    
    void extractSourceCodeAndCompile(const Shader &shader);

    unsigned int createShader(unsigned int shaderType);

    void compileShader(unsigned int shaderObject, const std::string & shader);

    void checkCompilationStatus(unsigned int shaderObject);

    void checkLinkStatus();

    void deleteShaders();
};

