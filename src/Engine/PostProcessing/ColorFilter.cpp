#include "ColorFilter.hpp"

#include <Video/Shader/Shader.hpp>
#include <Video/Shader/ShaderProgram.hpp>
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "Post.vert.hpp"
#include "PostColor.frag.hpp"

using namespace Video;

ColorFilter::ColorFilter(const glm::vec3& color) {
    vertexShader = Managers().resourceManager->CreateShader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(POSTCOLOR_FRAG, POSTCOLOR_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
    
    this->color = color;
    colorLocation = shaderProgram->GetUniformLocation("color");
}

ColorFilter::~ColorFilter() {
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
}

ShaderProgram* ColorFilter::GetShaderProgram() const {
    return shaderProgram;
}

void ColorFilter::SetUniforms() {
    glUniform3fv(colorLocation, 1, &color[0]);
}

void ColorFilter::SetColor(const glm::vec3& color) {
    this->color = color;
}
