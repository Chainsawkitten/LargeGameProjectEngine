#include "DebugDrawingManager.hpp"

#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Lens.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../MainWindow.hpp"

using namespace Video;

DebugDrawingManager::DebugDrawingManager() {
    debugDrawing = new DebugDrawing();
}

DebugDrawingManager::~DebugDrawingManager() {
    delete debugDrawing;
}

void DebugDrawingManager::AddPoint(const glm::vec3& position, const glm::vec3& color, float size, float duration, bool depthTesting) {
    DebugDrawing::Point point;
    point.position = position;
    point.color = color;
    point.size = size;
    point.duration = duration;
    point.depthTesting = depthTesting;
    points.push_back(point);
}

void DebugDrawingManager::AddLine(const glm::vec3& startPosition, const glm::vec3& endPosition, const glm::vec3& color, float width, float duration, bool depthTesting) {
    DebugDrawing::Line line;
    line.startPosition = startPosition;
    line.endPosition = endPosition;
    line.color = color;
    line.width = width;
    line.duration = duration;
    line.depthTesting = depthTesting;
    lines.push_back(line);
}

void DebugDrawingManager::AddCuboid(const glm::vec3& minCoordinates, const glm::vec3& maxCoordinates, const glm::vec3& color, float lineWidth, float duration, bool depthTesting) {
    DebugDrawing::Cuboid cuboid;
    cuboid.minCoordinates = minCoordinates;
    cuboid.maxCoordinates = maxCoordinates;
    cuboid.color = color;
    cuboid.lineWidth = lineWidth;
    cuboid.duration = duration;
    cuboid.depthTesting = depthTesting;
    cuboids.push_back(cuboid);
}

void DebugDrawingManager::Update(float deltaTime) {
    // Points.
    for (std::size_t i=0; i < points.size(); ++i) {
        if (points[i].duration < 0.f) {
            points[i] = points[points.size() - 1];
            points.pop_back();
            --i;
        } else {
            points[i].duration -= deltaTime;
        }
    }
    
    // Lines.
    for (std::size_t i=0; i < lines.size(); ++i) {
        if (lines[i].duration < 0.f) {
            lines[i] = lines[lines.size() - 1];
            lines.pop_back();
            --i;
        } else {
            lines[i].duration -= deltaTime;
        }
    }
    
    // Cuboid.
    for (std::size_t i=0; i < cuboids.size(); ++i) {
        if (cuboids[i].duration < 0.f) {
            cuboids[i] = cuboids[cuboids.size() - 1];
            cuboids.pop_back();
            --i;
        } else {
            cuboids[i].duration -= deltaTime;
        }
    }
}

void DebugDrawingManager::Render(World& world, Entity* camera) {
    // Find camera entity.
    if (camera == nullptr) {
        std::vector<Component::Lens*> lenses = world.GetComponents<Component::Lens>();
        for (Component::Lens* lens : lenses) {
            camera = lens->entity;
        }
    }
    
    if (camera != nullptr) {
        glm::mat4 viewMat(camera->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->GetWorldPosition()));
        glm::mat4 projectionMat(camera->GetComponent<Component::Lens>()->GetProjection(MainWindow::GetInstance()->GetSize()));
        glm::mat4 viewProjectionMatrix(projectionMat * viewMat);
        
        debugDrawing->StartDebugDrawing(viewProjectionMatrix);
        
        // Points.
        for (const DebugDrawing::Point& point : points) {
            debugDrawing->DrawPoint(point);
        }
        
        // Lines.
        for (const DebugDrawing::Line& line : lines)
            debugDrawing->DrawLine(line);
        
        // Cuboids.
        for (const DebugDrawing::Cuboid& cuboid : cuboids)
            debugDrawing->DrawCuboid(cuboid);
        
        debugDrawing->EndDebugDrawing();
    }
}
