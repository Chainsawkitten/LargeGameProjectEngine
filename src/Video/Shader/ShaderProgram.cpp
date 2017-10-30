#include "ShaderProgram.hpp"

#include "Shader.hpp"
#include "Utility/Log.hpp"

using namespace Video;

ShaderProgram::ShaderProgram(std::initializer_list<const Shader*> shaders) {
    shaderProgram = glCreateProgram();
   
    for (const Shader* shader : shaders)
        glAttachShader(shaderProgram, shader->GetShaderID());
    
    glLinkProgram(shaderProgram);
    
    for (const Shader* shader : shaders)
        glDetachShader(shaderProgram, shader->GetShaderID());
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(shaderProgram);
}

void ShaderProgram::Use() const {
    glUseProgram(shaderProgram);
}

GLuint ShaderProgram::GetUniformLocation(const char* name) const {
    return glGetUniformLocation(shaderProgram, name);
}
