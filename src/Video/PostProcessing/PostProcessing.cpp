#include "PostProcessing.hpp"

#include <Video/RenderTarget.hpp>
#include <Video/Shader/ShaderProgram.hpp>
#include <Video/Geometry/Rectangle.hpp>
#include <Video/PostProcessing/Filter.hpp>

using namespace Video;

PostProcessing::PostProcessing(const glm::vec2& screenSize, const Geometry::Rectangle* rectangle) {
    buffers[0] = new RenderTarget(screenSize);
    buffers[1] = new RenderTarget(screenSize);
    
    this->rectangle = rectangle;
}

PostProcessing::~PostProcessing() {
    delete buffers[0];
    delete buffers[1];
}

RenderTarget* PostProcessing::GetRenderTarget() const {
    return buffers[which];
}

void PostProcessing::UpdateBufferSize(const glm::vec2& screenSize){
    delete buffers[0];
    delete buffers[1];
    
    buffers[0] = new RenderTarget(screenSize);
    buffers[1] = new RenderTarget(screenSize);
}

void PostProcessing::ApplyFilter(Video::Filter* filter) {
    // Always pass depth test.
    glDepthFunc(GL_ALWAYS);
    
    buffers[1 - which]->SetTarget();
    
    filter->GetShaderProgram()->Use();
    
    glUniform1i(filter->GetShaderProgram()->GetUniformLocation("tDiffuse"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffers[which]->GetColorTexture());
    
    glUniform1i(filter->GetShaderProgram()->GetUniformLocation("tExtra"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, buffers[which]->GetExtraTexture());
    
    glUniform1i(filter->GetShaderProgram()->GetUniformLocation("tDepth"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, buffers[which]->GetDepthTexture());
    
    glBindVertexArray(rectangle->GetVertexArray());
    
    filter->SetUniforms();
    
    glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    
    // Reset depth testing to standard value.
    glDepthFunc(GL_LESS);
    
    which = 1 - which;
}

void PostProcessing::Render(bool dither) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    buffers[which]->Render(dither);
}
