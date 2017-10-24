#include "Renderer.hpp"

#include <GL/glew.h>
#include "RenderProgram/StaticRenderProgram.hpp"
#include "RenderProgram/SkinRenderProgram.hpp"
#include "RenderSurface.hpp"
#include "PostProcessing/PostProcessing.hpp"
#include "PostProcessing/FXAAFilter.hpp"
#include "Texture/Texture2D.hpp"
#include "Shader/Shader.hpp"
#include "Shader/ShaderProgram.hpp"
#include "EditorEntity.vert.hpp"
#include "EditorEntity.geom.hpp"
#include "EditorEntity.frag.hpp"
#include "Geometry/Rectangle.hpp"
#include "Buffer/FrameBuffer.hpp"
#include "Buffer/StorageBuffer.hpp"

using namespace Video;

Renderer::Renderer() {
    rectangle = new Geometry::Rectangle();
    staticRenderProgram = new StaticRenderProgram();

    postProcessing = new PostProcessing(rectangle);

    fxaaFilter = new FXAAFilter();

    lightCount = 0;
    lightBuffer = new StorageBuffer(sizeof(Video::Light), GL_DYNAMIC_DRAW);

    // Icon rendering.
    Shader* iconVertexShader = new Shader(EDITORENTITY_VERT, EDITORENTITY_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* iconGeometryShader = new Shader(EDITORENTITY_GEOM, EDITORENTITY_GEOM_LENGTH, GL_GEOMETRY_SHADER);
    Shader* iconFragmentShader = new Shader(EDITORENTITY_FRAG, EDITORENTITY_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    iconShaderProgram = new ShaderProgram({ iconVertexShader, iconGeometryShader, iconFragmentShader });
    delete iconVertexShader;
    delete iconGeometryShader;
    delete iconFragmentShader;

    // Create icon geometry.
    float vertex;

    glBindVertexArray(0);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(float), &vertex, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);

    glBindVertexArray(0);
}

Renderer::~Renderer() {
    delete rectangle;
    delete staticRenderProgram;

    delete postProcessing;
    delete fxaaFilter;

    delete lightBuffer;

    // Icon rendering.
    delete iconShaderProgram;
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
}

void Renderer::PrepareStaticMeshDepthRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    staticRenderProgram->PreDepthRender(viewMatrix, projectionMatrix);
}

void Renderer::DepthRenderStaticMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) {
    staticRenderProgram->DepthRender(geometry, viewMatrix, projectionMatrix, modelMatrix);
}

void Renderer::StartRendering(RenderSurface* renderSurface) {
    renderSurface->Clear();
    glViewport(0, 0, static_cast<GLsizei>(renderSurface->GetSize().x), static_cast<GLsizei>(renderSurface->GetSize().y));
}

void Renderer::SetLights(const std::vector<Video::Light>& lights) {
    lightCount = lights.size();

    // Skip if no lights.
    if (lightCount == 0)
        return;

    // Resize light buffer if necessary.
    unsigned int byteSize = sizeof(Video::Light) * lights.size();
    if (lightBuffer->GetSize() < byteSize) {
        delete lightBuffer;
        lightBuffer = new StorageBuffer(byteSize, GL_DYNAMIC_DRAW);
    }

    // Update light buffer.
    lightBuffer->Bind();
    lightBuffer->Write((void*)lights.data(), 0, byteSize);
    lightBuffer->Unbind();
}

void Renderer::PrepareStaticMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    staticRenderProgram->PreRender(viewMatrix, projectionMatrix, lightBuffer, lightCount);
}

void Renderer::RenderStaticMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* metallic, const Texture2D* roughness, const glm::mat4 modelMatrix, bool isSelected) {
    staticRenderProgram->Render(geometry, albedo, normal, metallic, roughness, modelMatrix, isSelected);
}

void Renderer::AntiAlias(RenderSurface* renderSurface) {
    fxaaFilter->SetScreenSize(renderSurface->GetSize());
    postProcessing->ApplyFilter(renderSurface, fxaaFilter);
}

void Renderer::Present(RenderSurface* renderSurface) {
    const glm::vec2 size = renderSurface->GetSize();

    /// @todo See if doing this with a fullscreen quad would be faster.
    /// With a fullscreen this would be one command (copying both color and depth) instead of two.
    /// Additionally, online sources seem to indicate fullscreen quads are slightly faster than blitting.

    // Copy color buffer.
    renderSurface->GetColorFrameBuffer()->BindRead();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    renderSurface->GetColorFrameBuffer()->Unbind();

    // Copy depth buffer.
    renderSurface->GetDepthFrameBuffer()->BindRead();
    glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    renderSurface->GetDepthFrameBuffer()->Unbind();
}

void Renderer::PrepareRenderingIcons(const glm::mat4& viewProjectionMatrix, const glm::vec3& cameraPosition, const glm::vec3& cameraUp) {
    iconShaderProgram->Use();
    glBindVertexArray(vertexArray);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Set camera uniforms.
    glUniformMatrix4fv(iconShaderProgram->GetUniformLocation("viewProjectionMatrix"), 1, GL_FALSE, &viewProjectionMatrix[0][0]);
    glUniform3fv(iconShaderProgram->GetUniformLocation("cameraPosition"), 1, &cameraPosition[0]);
    glUniform3fv(iconShaderProgram->GetUniformLocation("cameraUp"), 1, &cameraUp[0]);
    glUniform1i(iconShaderProgram->GetUniformLocation("baseImage"), 0);

    glActiveTexture(GL_TEXTURE0);
}

void Renderer::RenderIcon(const glm::vec3& position, const Texture2D* icon) {
    if (currentIcon != icon) {
        currentIcon = icon;
        glBindTexture(GL_TEXTURE_2D, icon->GetTextureID());
    }

    glUniform3fv(iconShaderProgram->GetUniformLocation("position"), 1, &position[0]);
    glDrawArrays(GL_POINTS, 0, 1);
}

void Renderer::StopRenderingIcons() {
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
