#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <Video/DebugDrawing.hpp>

class Entity;

/// Debug drawing facilities.
class DebugDrawingManager {
    friend class Hub;
    
    public:
        /// Add a point to the world.
        /**
         * @param position World position of the point.
         * @param color Color of the point.
         * @param size Size of the point.
         * @param duration How long the point should stay in the world (in seconds).
         * @param depthTesting Whether to enable depth testing.
         */
        void AddPoint(const glm::vec3& position, const glm::vec3& color, float size, float duration = 0.f, bool depthTesting = true);
        
        /// Add a line to the world.
        /**
         * @param startPosition Position of the start point of the line.
         * @param endPosition Position of the end point of the line.
         * @param color Color of the line.
         * @param width The width of the line.
         * @param duration How long the line should stay in the world (in seconds).
         * @param depthTesting Whether to enable depth testing.
         */
        void AddLine(const glm::vec3& startPosition, const glm::vec3& endPosition, const glm::vec3& color, float width = 1.f, float duration = 0.f, bool depthTesting = true);
        
        /// Add a cuboid to the world.
        /**
         * @param minCoordinates The minimum coordinates of the box.
         * @param maxCoordinates The maximum coordinates of the box.
         * @param color Color of the lines.
         * @param lineWidth The width of the lines used to draw the box.
         * @param duration How long the box should stay in the world (in seconds).
         * @param depthTesting Whether to enable depth testing.
         */
        void AddCuboid(const glm::vec3& minCoordinates, const glm::vec3& maxCoordinates, const glm::vec3& color, float lineWidth = 1.f, float duration = 0.f, bool depthTesting = true);
        
        /// Add a plane to the world.
        /**
         * @param position Center position of the plane.
         * @param normal Plane normal.
         * @param size Size.
         * @param color Color of the lines.
         * @param lineWidth The width of the lines used to draw the plane.
         * @param duration How long the plane should stay in the world (in seconds).
         * @param depthTesting Whether to enable depth testing.
         */
        void AddPlane(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& size, const glm::vec3& color, float lineWidth = 1.f, float duration = 0.f, bool depthTesting = true);
        
        /// Add a sphere to the world.
        /**
         * @param position The position of the sphere.
         * @param radius The radius of the sphere.
         * @param color Color of the lines.
         * @param lineWidth The width of the lines used to draw the plane.
         * @param duration How long the plane should stay in the world (in seconds).
         * @param depthTesting Whether to enable depth testing.
         */
        void AddSphere(const glm::vec3& position, float radius, const glm::vec3& color, float lineWidth = 1.f, float duration = 0.f, bool depthTesting = true);
        
        /// Update the debug geometry.
        /**
         * @param deltaTime Time since last frame (in seconds).
         */
        void Update(float deltaTime);
        
        /// Render the debug primitives.
        /**
         * @param camera Camera through which to render (or first camera in world if nullptr).
         */
        void Render(Entity* camera);
        
    private:
        DebugDrawingManager();
        ~DebugDrawingManager();
        DebugDrawingManager(DebugDrawingManager const&) = delete;
        void operator=(DebugDrawingManager const&) = delete;
        
        std::vector<Video::DebugDrawing::Point> points;
        std::vector<Video::DebugDrawing::Line> lines;
        std::vector<Video::DebugDrawing::Cuboid> cuboids;
        std::vector<Video::DebugDrawing::Plane> planes;
        std::vector<Video::DebugDrawing::Sphere> spheres;
        
        Video::DebugDrawing* debugDrawing;
};
