#include "Renderer.hpp"

#include <GL/glew.h>
#include "Lighting/Light.hpp"
#include "Lighting/Lighting.hpp"
#include "RenderProgram/StaticRenderProgram.hpp"
#include "RenderProgram/SkinRenderProgram.hpp"
#include "RenderSurface.hpp"
#include "PostProcessing/PostProcessing.hpp"
#include "PostProcessing/ColorFilter.hpp"
#include "PostProcessing/FogFilter.hpp"
#include "PostProcessing/FXAAFilter.hpp"
#include "PostProcessing/GammaCorrectionFilter.hpp"
#include "PostProcessing/GlowBlurFilter.hpp"
#include "PostProcessing/GlowFilter.hpp"
#include "Texture/Texture2D.hpp"
#include "Shader/Shader.hpp"
#include "Shader/ShaderProgram.hpp"
#include "EditorEntity.vert.hpp"
#include "EditorEntity.geom.hpp"
#include "EditorEntity.frag.hpp"
#include "Geometry/Rectangle.hpp"
#include "FrameBuffer.hpp"

using namespace Video;

Renderer::Renderer() {
    rectangle = new Geometry::Rectangle();
    staticRenderProgram = new StaticRenderProgram();
    lighting = new Lighting(staticRenderProgram->GetShaderProgram(), rectangle);
   // skinRenderProgram = new SkinRenderProgram();
   // postProcessing = new PostProcessing(rectangle);
   // colorFilter = new ColorFilter(glm::vec3(1.f, 1.f, 1.f));
   // fogFilter = new FogFilter(glm::vec3(1.f, 1.f, 1.f));
   // fxaaFilter = new FXAAFilter();
   // glowFilter = new GlowFilter();
   // glowBlurFilter = new GlowBlurFilter();
    
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
    delete lighting;
    delete staticRenderProgram;
    //delete skinRenderProgram;
    //delete postProcessing;
    //delete colorFilter;
    //delete fogFilter;
    //delete fxaaFilter;
    //delete glowFilter;
    //delete glowBlurFilter;
    
    // Icon rendering.
    delete iconShaderProgram;
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
}

void Renderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Video::Renderer::DepthRenderStaticMesh(Geometry::Geometry3D * geometry, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix, const glm::mat4 modelMatrix)
{
    staticRenderProgram->DepthRender(geometry, viewMatrix, projectionMatrix, modelMatrix);
}

void Renderer::StartRendering(RenderSurface* renderSurface) {
    lighting->ClearLights();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, static_cast<GLsizei>(renderSurface->GetSize().x), static_cast<GLsizei>(renderSurface->GetSize().y));
}

void Renderer::AddLight(const Video::Light& light) {
    lighting->AddLight(light);
}

void Renderer::Light(const glm::mat4& inverseProjectionMatrix, RenderSurface* renderSurface) {
    lighting->Render(inverseProjectionMatrix, renderSurface);
}

void Renderer::PrepareStaticMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    staticRenderProgram->PreRender(viewMatrix, projectionMatrix);
}

void Renderer::RenderStaticMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* metallic, const Texture2D* roughness, const glm::mat4 modelMatrix, bool isSelected) {
    staticRenderProgram->Render(geometry, albedo, normal, metallic, roughness, modelMatrix, isSelected);
}

void Renderer::AntiAlias(RenderSurface* renderSurface) {
    //fxaaFilter->SetScreenSize(renderSurface->GetSize());
    //postProcessing->ApplyFilter(renderSurface, fxaaFilter);
}

void Renderer::RenderFog(RenderSurface* renderSurface, const glm::mat4& projectionMatrix, float density, const glm::vec3& color) {
    /*fogFilter->SetProjectionMatrix(projectionMatrix);
    fogFilter->SetDensity(density);
    fogFilter->SetColor(color);
    postProcessing->ApplyFilter(renderSurface, fogFilter);*/
}

void Renderer::ApplyGlow(RenderSurface* renderSurface, int blurAmount) {
    /*glowBlurFilter->SetScreenSize(renderSurface->GetSize());
    for (int i = 0; i < blurAmount; ++i) {
        glowBlurFilter->SetHorizontal(true);
        postProcessing->ApplyFilter(renderSurface, glowBlurFilter);
        glowBlurFilter->SetHorizontal(false);
        postProcessing->ApplyFilter(renderSurface, glowBlurFilter);
    }
    postProcessing->ApplyFilter(renderSurface, glowFilter);*/
}

void Renderer::ApplyColorFilter(RenderSurface* renderSurface, const glm::vec3& color) {
    /*colorFilter->SetColor(color);
    postProcessing->ApplyFilter(renderSurface, colorFilter);*/
}

void Renderer::DisplayResults(RenderSurface* renderSurface, bool dither) {
    //postProcessing->Render(renderSurface, dither);
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

