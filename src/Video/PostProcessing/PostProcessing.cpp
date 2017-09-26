#include "PostProcessing.hpp"

#include <Video/Shader/ShaderProgram.hpp>
#include <Video/Geometry/Rectangle.hpp>
#include <Video/PostProcessing/Filter.hpp>
#include "Shader/Shader.hpp"
#include <PostCopy.frag.hpp>
#include <Post.vert.hpp>
#include <PostDither.frag.hpp>
#include "RenderSurface.hpp"
#include "ReadWriteTexture.hpp"
#include "FrameBuffer.hpp"

using namespace Video;

PostProcessing::PostProcessing(const Geometry::Rectangle* rectangle) {
    
    this->rectangle = rectangle;

    Shader* vertexShader = new Shader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(POSTCOPY_FRAG, POSTCOPY_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
    delete fragmentShader;

    Shader* ditherFragmentShader = new Shader(POSTDITHER_FRAG, POSTDITHER_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    ditherShaderProgram = new ShaderProgram({ vertexShader, ditherFragmentShader });

    delete vertexShader;
    delete ditherFragmentShader;
}

PostProcessing::~PostProcessing() {
    delete shaderProgram;
    delete ditherShaderProgram;
}

void PostProcessing::ApplyFilter(Video::RenderSurface* renderSurface, Video::Filter* filter) const {
    // Always pass depth test.
    glDepthFunc(GL_ALWAYS);

    // Swap render surface read/write buffers.
    renderSurface->Swap();

    // Set render surface as render target.
    renderSurface->GetPostProcessingFrameBuffer()->SetTarget();

    // Bind shaders.
    filter->GetShaderProgram()->Use();
    
    glUniform1i(filter->GetShaderProgram()->GetUniformLocation("tDiffuse"), 0);
    renderSurface->GetColorTexture()->BindForReading(GL_TEXTURE0);

    glUniform1i(filter->GetShaderProgram()->GetUniformLocation("tExtra"), 1);
    renderSurface->GetExtraColorTexture()->BindForReading(GL_TEXTURE1);

    glUniform1i(filter->GetShaderProgram()->GetUniformLocation("tDepth"), 2);
    renderSurface->GetExtraDepthTexture()->BindForReading(GL_TEXTURE2);
    
    glBindVertexArray(rectangle->GetVertexArray());
    
    filter->SetUniforms();
    
    glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    
    // Reset depth testing to standard value.
    glDepthFunc(GL_LESS);
}

void PostProcessing::Render(RenderSurface* renderSurface, bool dither) {
    // Render to backbuffer.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Always pass depth test.
    glDepthFunc(GL_ALWAYS);

    ShaderProgram* shader = dither ? ditherShaderProgram : shaderProgram;
    shader->Use();

    // Swap render surface read/write buffers.
    renderSurface->Swap();

    glUniform1i(shader->GetUniformLocation("tDiffuse"), 0);
    renderSurface->GetColorTexture()->BindForReading(GL_TEXTURE0);

    glUniform1i(shader->GetUniformLocation("tDepth"), 1);
    renderSurface->GetExtraDepthTexture()->BindForReading(GL_TEXTURE1);

    if (dither) {
        glUniform1f(shader->GetUniformLocation("time"), ditherTime);
        ditherTime = fmod(ditherTime + 1.f, 255.f);
    }

    glBindVertexArray(rectangle->GetVertexArray());

    glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);

    // Reset depth testing to standard value.
    glDepthFunc(GL_LESS);
}
