#include "ParticleRenderer.hpp"

#include "Texture/Texture.hpp"
#include "Shader/Shader.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Particle.vert.hpp"
#include "Particle.geom.hpp"
#include "Particle.frag.hpp"

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

using namespace Video;

ParticleRenderer::ParticleRenderer(unsigned int maxParticleCount) {
    // Load shaders.
    Shader* vertexShader = new Shader(PARTICLE_VERT, PARTICLE_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* geometryShader = new Shader(PARTICLE_GEOM, PARTICLE_GEOM_LENGTH, GL_GEOMETRY_SHADER);
    Shader* fragmentShader = new Shader(PARTICLE_FRAG, PARTICLE_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = new ShaderProgram({ vertexShader, geometryShader, fragmentShader });
    delete vertexShader;
    delete geometryShader;
    delete fragmentShader;

    // Vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxParticleCount * sizeof(Video::ParticleRenderer::Particle), NULL, GL_DYNAMIC_DRAW);

    // Define vertex data layout.
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Video::ParticleRenderer::Particle), BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Video::ParticleRenderer::Particle), BUFFER_OFFSET(sizeof(float) * 3));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Video::ParticleRenderer::Particle), BUFFER_OFFSET(sizeof(float) * 5));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Video::ParticleRenderer::Particle), BUFFER_OFFSET(sizeof(float) * 6));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Video::ParticleRenderer::Particle), BUFFER_OFFSET(sizeof(float) * 7));
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Video::ParticleRenderer::Particle), BUFFER_OFFSET(sizeof(float) * 10));
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Video::ParticleRenderer::Particle), BUFFER_OFFSET(sizeof(float) * 13));
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(Video::ParticleRenderer::Particle), BUFFER_OFFSET(sizeof(float) * 16));

    glBindVertexArray(0);
}

ParticleRenderer::~ParticleRenderer() {
    delete shaderProgram;

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
}

void ParticleRenderer::SetBufferContents(unsigned int count, const Particle* particles) {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Video::ParticleRenderer::Particle), particles);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    particleCount = count;
}

void ParticleRenderer::Render(Texture* textureAtlas, unsigned int textureAtlasRows, const glm::vec3& cameraPosition, const glm::vec3& cameraUp, const glm::mat4& viewProjectionMatrix) {
    // Don't write to depth buffer.
    GLboolean depthWriting;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriting);
    glDepthMask(GL_FALSE);

    // Blending
    glEnablei(GL_BLEND, 0);
    glEnablei(GL_BLEND, 1);
    glBlendFunci(0, GL_SRC_ALPHA, GL_ONE);
    glBlendFunci(1, GL_SRC_ALPHA, GL_ONE);

    shaderProgram->Use();

    glBindVertexArray(vertexArray);

    glUniform1i(shaderProgram->GetUniformLocation("baseImage"), 0);

    // Base image texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureAtlas->GetTextureID());

    // Send the matrices to the shader.
    glUniform3fv(shaderProgram->GetUniformLocation("cameraPosition"), 1, &cameraPosition[0]);
    glUniform3fv(shaderProgram->GetUniformLocation("cameraUp"), 1, &cameraUp[0]);
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("viewProjectionMatrix"), 1, GL_FALSE, &viewProjectionMatrix[0][0]);
    glUniform1f(shaderProgram->GetUniformLocation("textureAtlasRows"), textureAtlasRows);

    // Draw the triangles
    glDrawArrays(GL_POINTS, 0, particleCount);

    // Reset state values we've changed.
    glDepthMask(depthWriting);
    glDisablei(GL_BLEND, 0);
    glDisablei(GL_BLEND, 1);

    glUseProgram(0);
    glBindVertexArray(0);
}
