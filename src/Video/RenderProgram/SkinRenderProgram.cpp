#include "SkinRenderProgram.hpp"

#include "../Geometry/Geometry3D.hpp"
#include "../Texture/Texture2D.hpp"
#include "../Culling/Frustum.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "Skinning.vert.hpp"
#include "Default3D.frag.hpp"

using namespace Video;

SkinRenderProgram::SkinRenderProgram() {
    Shader* vertexShader = new Shader(SKINNING_VERT, SKINNING_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(DEFAULT3D_FRAG, DEFAULT3D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
    delete vertexShader;
    delete fragmentShader;
}

SkinRenderProgram::~SkinRenderProgram() {
    delete shaderProgram;
}

void SkinRenderProgram::PreRender(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    shaderProgram->Use();

    this->viewMatrix = viewMatrix;
    this->projectionMatrix = projectionMatrix;
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    glUniformMatrix4fv(shaderProgram->GetUniformLocation("viewProjection"), 1, GL_FALSE, &viewProjectionMatrix[0][0]);
}

void SkinRenderProgram::Render(const Geometry::Geometry3D* geometry, const Texture2D* textureAlbedo, const Texture2D* textureNormal, const Texture2D* textureMetallic, const Texture2D* textureRoughness, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones, const std::vector<glm::mat3>& bonesIT) const {
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
        glBindTexture(GL_TEXTURE_2D, textureNormal->GetTextureID());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureMetallic->GetTextureID());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureRoughness->GetTextureID());

        // Render model.
        glUniformMatrix4fv(shaderProgram->GetUniformLocation("model"), 1, GL_FALSE, &modelMatrix[0][0]);
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(viewMatrix * modelMatrix));
        glUniformMatrix3fv(shaderProgram->GetUniformLocation("normalMatrix"), 1, GL_FALSE, &glm::mat3(normalMatrix)[0][0]);
        assert(bones.size() <= 100 && bonesIT.size() <= 100);
        glUniformMatrix4fv(shaderProgram->GetUniformLocation("bones"), bones.size(), GL_FALSE, &bones[0][0][0]);
        glUniformMatrix3fv(shaderProgram->GetUniformLocation("bonesIT"), bonesIT.size(), GL_FALSE, &bonesIT[0][0][0]);

        glDrawElements(GL_TRIANGLES, geometry->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    }
}
