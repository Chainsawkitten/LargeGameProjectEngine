#include "DebugDrawing.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "Shader/Shader.hpp"
#include "Shader/ShaderProgram.hpp"
#include "DebugDrawing.vert.hpp"
#include "DebugDrawing.frag.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

using namespace Video;

DebugDrawing::DebugDrawing() {
    Shader* vertexShader = new Shader(DEBUGDRAWING_VERT, DEBUGDRAWING_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(DEBUGDRAWING_FRAG, DEBUGDRAWING_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
    delete vertexShader;
    delete fragmentShader;
    
    // Create point vertex array.
    glBindVertexArray(0);
    
    glGenBuffers(1, &pointVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, pointVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(glm::vec3), &glm::vec3(0.f, 0.f, 0.f)[0], GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &pointVertexArray);
    glBindVertexArray(pointVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, pointVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
    
    // Create line vertex array.
    glm::vec3 line[2];
    line[0] = glm::vec3(0.f, 0.f, 0.f);
    line[1] = glm::vec3(1.f, 1.f, 1.f);
    
    glGenBuffers(1, &lineVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, lineVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), line, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &lineVertexArray);
    glBindVertexArray(lineVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, lineVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
    
    // Create cuboid vertex array.
    glm::vec3 box[24];
    box[0] = glm::vec3(0.f, 0.f, 0.f);
    box[1] = glm::vec3(1.f, 0.f, 0.f);
    box[2] = glm::vec3(1.f, 0.f, 0.f);
    box[3] = glm::vec3(1.f, 1.f, 0.f);
    box[4] = glm::vec3(1.f, 1.f, 0.f);
    box[5] = glm::vec3(0.f, 1.f, 0.f);
    box[6] = glm::vec3(1.f, 1.f, 0.f);
    box[7] = glm::vec3(1.f, 1.f, 1.f);
    box[8] = glm::vec3(1.f, 1.f, 1.f);
    box[9] = glm::vec3(1.f, 0.f, 1.f);
    box[10] = glm::vec3(1.f, 0.f, 1.f);
    box[11] = glm::vec3(1.f, 0.f, 0.f);
    box[12] = glm::vec3(0.f, 1.f, 0.f);
    box[13] = glm::vec3(0.f, 1.f, 1.f);
    box[14] = glm::vec3(0.f, 1.f, 1.f);
    box[15] = glm::vec3(0.f, 0.f, 1.f);
    box[16] = glm::vec3(0.f, 1.f, 0.f);
    box[17] = glm::vec3(0.f, 0.f, 0.f);
    box[18] = glm::vec3(0.f, 0.f, 1.f);
    box[19] = glm::vec3(0.f, 0.f, 0.f);
    box[20] = glm::vec3(0.f, 1.f, 1.f);
    box[21] = glm::vec3(1.f, 1.f, 1.f);
    box[22] = glm::vec3(0.f, 0.f, 1.f);
    box[23] = glm::vec3(1.f, 0.f, 1.f);
    
    glGenBuffers(1, &cuboidVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cuboidVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec3), box, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &cuboidVertexArray);
    glBindVertexArray(cuboidVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, cuboidVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
}

DebugDrawing::~DebugDrawing() {
    glDeleteBuffers(1, &cuboidVertexBuffer);
    glDeleteVertexArrays(1, &cuboidVertexArray);
    
    glDeleteBuffers(1, &pointVertexBuffer);
    glDeleteVertexArrays(1, &pointVertexArray);
    
    glDeleteBuffers(1, &lineVertexBuffer);
    glDeleteVertexArrays(1, &lineVertexArray);
    
    delete shaderProgram;
}

void DebugDrawing::StartDebugDrawing(const glm::mat4& viewProjectionMatrix) {
    shaderProgram->Use();
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("viewProjection"), 1, GL_FALSE, &viewProjectionMatrix[0][0]);
}

void DebugDrawing::DrawPoint(const Point& point) {
    BindVertexArray(pointVertexArray);
    
    glm::mat4 model(glm::translate(glm::mat4(), point.position));
    
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);
    point.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(shaderProgram->GetUniformLocation("color"), 1, &point.color[0]);
    glUniform1f(shaderProgram->GetUniformLocation("size"), point.size);
    glDrawArrays(GL_POINTS, 0, 1);
}

void DebugDrawing::DrawLine(const Line& line) {
    BindVertexArray(lineVertexArray);
    
    glm::mat4 model(glm::translate(glm::mat4(), line.startPosition) * glm::scale(glm::mat4(), line.endPosition - line.startPosition));
    
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);
    line.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(shaderProgram->GetUniformLocation("color"), 1, &line.color[0]);
    glLineWidth(line.width);
    glDrawArrays(GL_LINES, 0, 2);
}

void DebugDrawing::DrawCuboid(const Cuboid& cuboid) {
    BindVertexArray(cuboidVertexArray);
    
    glm::mat4 model(glm::translate(glm::mat4(), cuboid.minCoordinates) * glm::scale(glm::mat4(), cuboid.maxCoordinates - cuboid.minCoordinates));
    
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);
    cuboid.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(shaderProgram->GetUniformLocation("color"), 1, &cuboid.color[0]);
    glUniform1f(shaderProgram->GetUniformLocation("size"), 10.f);
    glLineWidth(cuboid.lineWidth);
    glDrawArrays(GL_LINES, 0, 24);
}

void DebugDrawing::EndDebugDrawing() {
    glEnable(GL_DEPTH_TEST);
    BindVertexArray(0);
}

void DebugDrawing::BindVertexArray(GLuint vertexArray) {
    if (boundVertexArray != vertexArray) {
        glBindVertexArray(vertexArray);
        boundVertexArray = vertexArray;
    }
}
