#include "DebugDrawing.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "Shader/Shader.hpp"
#include "Shader/ShaderProgram.hpp"
#include "DebugDrawing.vert.hpp"
#include "DebugDrawing.frag.hpp"
#include "glm/gtc/constants.hpp"

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
    
    glm::vec3 point(0.f, 0.f, 0.f);
    CreateVertexArray(&point, 1, pointVertexBuffer, pointVertexArray);
    
    // Create line vertex array.
    glm::vec3 line[2];
    line[0] = glm::vec3(0.f, 0.f, 0.f);
    line[1] = glm::vec3(1.f, 1.f, 1.f);
    
    CreateVertexArray(line, 2, lineVertexBuffer, lineVertexArray);
    
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
    
    CreateVertexArray(box, 24, cuboidVertexBuffer, cuboidVertexArray);
    
    // Create plane vertex array.
    glm::vec3 plane[8];
    plane[0] = glm::vec3(-1.f, -1.f, 0.f);
    plane[1] = glm::vec3(1.f, -1.f, 0.f);
    plane[2] = glm::vec3(1.f, -1.f, 0.f);
    plane[3] = glm::vec3(1.f, 1.f, 0.f);
    plane[4] = glm::vec3(1.f, 1.f, 0.f);
    plane[5] = glm::vec3(-1.f, 1.f, 0.f);
    plane[6] = glm::vec3(-1.f, 1.f, 0.f);
    plane[7] = glm::vec3(-1.f, -1.f, 0.f);
    
    CreateVertexArray(plane, 8, planeVertexBuffer, planeVertexArray);
    
    // Create sphere vertex array.
    glm::vec3* sphere;
    CreateSphere(sphere, sphereVertexCount, 14);
    CreateVertexArray(sphere, sphereVertexCount, sphereVertexBuffer, sphereVertexArray);
    delete[] sphere;
}

DebugDrawing::~DebugDrawing() {
    glDeleteBuffers(1, &cuboidVertexBuffer);
    glDeleteVertexArrays(1, &cuboidVertexArray);
    
    glDeleteBuffers(1, &pointVertexBuffer);
    glDeleteVertexArrays(1, &pointVertexArray);
    
    glDeleteBuffers(1, &lineVertexBuffer);
    glDeleteVertexArrays(1, &lineVertexArray);
    
    glDeleteBuffers(1, &planeVertexBuffer);
    glDeleteVertexArrays(1, &planeVertexArray);
    
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

void DebugDrawing::DrawPlane(const Plane& plane) {
    BindVertexArray(planeVertexArray);
    
    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(plane.size * 0.5f, 1.f)));
    float yaw = atan2(plane.normal.x, plane.normal.z);
    float pitch = atan2(plane.normal.y, sqrt(plane.normal.x * plane.normal.x + plane.normal.z * plane.normal.z));
    model = glm::rotate(glm::mat4(), yaw, glm::vec3(0.f, 1.f, 0.f)) * model;
    model = glm::rotate(glm::mat4(), pitch, glm::vec3(1.f, 0.f, 0.f)) * model;
    model = glm::translate(glm::mat4(), plane.position) * model;
    
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);
    plane.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(shaderProgram->GetUniformLocation("color"), 1, &plane.color[0]);
    glUniform1f(shaderProgram->GetUniformLocation("size"), 10.f);
    glLineWidth(plane.lineWidth);
    glDrawArrays(GL_LINES, 0, 8);
}

void DebugDrawing::DrawSphere(const Sphere& sphere) {
    BindVertexArray(sphereVertexArray);
    
    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(sphere.radius, sphere.radius, sphere.radius)));
    model = glm::translate(glm::mat4(), sphere.position) * model;
    
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);
    sphere.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(shaderProgram->GetUniformLocation("color"), 1, &sphere.color[0]);
    glUniform1f(shaderProgram->GetUniformLocation("size"), 10.f);
    glLineWidth(sphere.lineWidth);
    glDrawArrays(GL_LINES, 0, sphereVertexCount);
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

void DebugDrawing::CreateVertexArray(const glm::vec3* positions, unsigned int positionCount, GLuint& vertexBuffer, GLuint& vertexArray) {
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, positionCount * sizeof(glm::vec3), positions, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
}

// Create UV-sphere with given number of parallel and meridian lines.
void DebugDrawing::CreateSphere(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail) {
    vertexCount = detail * (4 * detail - 2);
    positions = new glm::vec3[vertexCount];
    
    // Horizontal lines (meridians).
    unsigned int i = 0;
    for (unsigned int m = 1; m < detail; ++m) {
        float meridian = glm::pi<float>() * m / detail;
        for (unsigned int p = 0; p <= detail; ++p) {
            float parallel = 2.0f * glm::pi<float>() * p / detail;
            float angle = glm::pi<float>() * 0.5f - meridian;
            float y = sin(angle);
            float x = cos(angle);
            positions[i++] = glm::vec3(x * cos(parallel), y, x * sin(parallel));
            if (p > 0 && p < detail)
                positions[i++] = glm::vec3(x * cos(parallel), y, x * sin(parallel));
        }
    }
    
    // Vertical lines (parallels).
    for (unsigned int p = 0; p < detail; ++p) {
        float parallel = 2.0f * glm::pi<float>() * p / detail;
        for (unsigned int m = 0; m <= detail; ++m) {
            float meridian = glm::pi<float>() * m / detail;
            float angle = glm::pi<float>() * 0.5f - meridian;
            float y = sin(angle);
            float x = cos(angle);
            positions[i++] = glm::vec3(x * cos(parallel), y, x * sin(parallel));
            if (m > 0 && m < detail)
                positions[i++] = glm::vec3(x * cos(parallel), y, x * sin(parallel));
        }
    }
}
