#include "Lighting.hpp"

#include <Utility/Log.hpp>
#include "../Geometry/Rectangle.hpp"
#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "Post.vert.hpp"
#include "Deferred.frag.hpp"

using namespace Video;

Lighting::Lighting(const glm::vec2& screenSize, const Geometry::Rectangle* rectangle) {
    // Compile shader program.
    Shader* vertexShader = new Shader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(DEFERRED_FRAG, DEFERRED_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
    delete vertexShader;
    delete fragmentShader;
    
    this->rectangle = rectangle;
    
    // Create the FBO
    glGenFramebuffers(1, &frameBufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
    
    // Generate textures
    glGenTextures(NUM_TEXTURES, textures);
    glGenTextures(1, &depthHandle);
    
    unsigned int width = static_cast<unsigned int>(screenSize.x);
    unsigned int height = static_cast<unsigned int>(screenSize.y);
    AttachTexture(textures[DIFFUSE], width, height, GL_COLOR_ATTACHMENT0 + DIFFUSE, GL_RGB16F);
    AttachTexture(textures[NORMAL], width, height, GL_COLOR_ATTACHMENT0 + NORMAL, GL_RGB16F);
    AttachTexture(textures[SPECULAR], width, height, GL_COLOR_ATTACHMENT0 + SPECULAR, GL_RGB);
    AttachTexture(textures[GLOW], width, height, GL_COLOR_ATTACHMENT0 + GLOW, GL_RGB);
    
    // Bind depth handle
    glBindTexture(GL_TEXTURE_2D, depthHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthHandle, 0);
    
    // Create and intialize draw buffers (output from geometry pass)
    GLenum drawBuffers[NUM_TEXTURES];
    for (unsigned int i = 0; i < NUM_TEXTURES; i++)
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers(NUM_TEXTURES, drawBuffers);
    
    // Check if framebuffer created correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log() << "Frame buffer creation failed\n";
    
    // Default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    // Store light uniform locations.
    for (unsigned int lightIndex = 0; lightIndex < lightCount; ++lightIndex) {
        lightUniforms[lightIndex].position = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].position").c_str());
        lightUniforms[lightIndex].intensities = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].intensities").c_str());
        lightUniforms[lightIndex].attenuation = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].attenuation").c_str());
        lightUniforms[lightIndex].ambientCoefficient = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].ambientCoefficient").c_str());
        lightUniforms[lightIndex].coneAngle = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].coneAngle").c_str());
        lightUniforms[lightIndex].direction = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].direction").c_str());
    }
}

Lighting::~Lighting() {
    if (frameBufferObject != 0)
        glDeleteFramebuffers(1, &frameBufferObject);
    
    if (textures[0] != 0)
        glDeleteTextures(NUM_TEXTURES, textures);
    
    if (depthHandle != 0)
        glDeleteTextures(1, &depthHandle);
    
    delete shaderProgram;
}

void Lighting::SetTarget() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObject);
}

void Lighting::ClearLights() {
    lights.clear();
}

void Lighting::AddLight(const Light& light) {
    lights.push_back(light);
}

void Lighting::Render(const glm::mat4& inverseProjectionMatrix) {
    // Disable depth testing
    GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    
    GLint oldDepthFunctionMode;
    glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunctionMode);
    glDepthFunc(GL_ALWAYS);
    
    // Blending enabled for handling multiple light sources
    GLboolean blend = glIsEnabledi(GL_BLEND, 0);
    glEnablei(GL_BLEND, 0);
    glBlendEquationi(0, GL_FUNC_ADD);
    glBlendFunci(0, GL_ONE, GL_ONE);
    
    shaderProgram->Use();
    
    BindForReading();
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindVertexArray(rectangle->GetVertexArray());
    
    // Set uniforms.
    glUniform1i(shaderProgram->GetUniformLocation("tDiffuse"), Lighting::DIFFUSE);
    glUniform1i(shaderProgram->GetUniformLocation("tNormals"), Lighting::NORMAL);
    glUniform1i(shaderProgram->GetUniformLocation("tSpecular"), Lighting::SPECULAR);
    glUniform1i(shaderProgram->GetUniformLocation("tGlow"), Lighting::GLOW);
    glUniform1i(shaderProgram->GetUniformLocation("tDepth"), Lighting::NUM_TEXTURES);
    glUniform1i(shaderProgram->GetUniformLocation("lightCount"), lightCount);
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("inverseProjectionMatrix"), 1, GL_FALSE, &inverseProjectionMatrix[0][0]);
    
    // Render lights.
    unsigned int lightIndex = 0U;
    for (const Light& light : lights) {
        glUniform4fv(lightUniforms[lightIndex].position, 1, &light.position[0]);
        glUniform3fv(lightUniforms[lightIndex].intensities, 1, &light.intensities[0]);
        glUniform1f(lightUniforms[lightIndex].attenuation, light.attenuation);
        glUniform1f(lightUniforms[lightIndex].ambientCoefficient, light.ambientCoefficient);
        glUniform1f(lightUniforms[lightIndex].coneAngle, light.coneAngle);
        glUniform3fv(lightUniforms[lightIndex].direction, 1, &light.direction[0]);
        
        if (++lightIndex >= lightCount) {
            lightIndex = 0U;
            glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
        }
    }
    
    if (lightIndex != 0U) {
        glUniform1i(shaderProgram->GetUniformLocation("lightCount"), lightIndex);
        glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    }
    
    if (!depthTest)
        glDisable(GL_DEPTH_TEST);
    if (!blend)
        glDisablei(GL_BLEND, 0);
    
    glDepthFunc(oldDepthFunctionMode);
}

void Lighting::AttachTexture(GLuint texture, unsigned int width, unsigned int height, GLenum attachment, GLint internalFormat) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
}

void Lighting::BindForReading() const {
    for (unsigned int i = 0; i < NUM_TEXTURES; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
    
    glActiveTexture(GL_TEXTURE0 + NUM_TEXTURES);
    glBindTexture(GL_TEXTURE_2D, depthHandle);
}
