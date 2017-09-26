#include "StaticRenderProgram.hpp"

#include "../Geometry/Geometry3D.hpp"
#include "../Texture/Texture2D.hpp"
#include "../Culling/Frustum.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "Default3D.vert.hpp"
#include "Default3D.frag.hpp"
#include "Engine/Util/Profiling.hpp"

using namespace Video;

StaticRenderProgram::StaticRenderProgram() {
    Shader* vertexShader = new Shader(DEFAULT3D_VERT, DEFAULT3D_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(DEFAULT3D_FRAG, DEFAULT3D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
    delete vertexShader;
    delete fragmentShader;
}

StaticRenderProgram::~StaticRenderProgram() {
    delete shaderProgram;
}

void StaticRenderProgram::PreRender(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    shaderProgram->Use();
    
    this->viewMatrix = viewMatrix;
    this->projectionMatrix = projectionMatrix;
    viewProjectionMatrix = projectionMatrix * viewMatrix;
    
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("viewProjection"), 1, GL_FALSE, &viewProjectionMatrix[0][0]);
}

void StaticRenderProgram::Render(Geometry::Geometry3D* geometry, const Video::Texture2D* textureAlbedo, const Video::Texture2D* normalTexture, const Video::Texture2D* textureMetallic, const Video::Texture2D* textureRoughness, const glm::mat4 modelMatrix) const {
    Frustum frustum(viewProjectionMatrix * modelMatrix);
    if (frustum.Collide(geometry->GetAxisAlignedBoundingBox())) {
        glBindVertexArray(geometry->GetVertexArray());

        // Set texture locations
        glUniform1i(shaderProgram->GetUniformLocation("mapAlbedo"), 0);
        glUniform1i(shaderProgram->GetUniformLocation("mapNormal"), 1);
        glUniform1i(shaderProgram->GetUniformLocation("mapMetallic"), 2);
        glUniform1i(shaderProgram->GetUniformLocation("mapRoughness"), 3);
        
        // Textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureAlbedo->GetTextureID());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture->GetTextureID());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureMetallic->GetTextureID());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureRoughness->GetTextureID());

        // Render model.
        glUniformMatrix4fv(shaderProgram->GetUniformLocation("model"), 1, GL_FALSE, &modelMatrix[0][0]);
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(viewMatrix * modelMatrix));
        glUniformMatrix3fv(shaderProgram->GetUniformLocation("normalMatrix"), 1, GL_FALSE, &glm::mat3(normalMatrix)[0][0]);

        glDrawElements(GL_TRIANGLES, geometry->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    }
}
