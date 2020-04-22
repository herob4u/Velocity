/*
 * ShaderProgram
 */

#pragma once

#include <string>


class ShaderProgram {
public:
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

    int getUniformLocation(const char * uniformName) const;

    int getAttribLocation(const char * attributeName) const;


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

