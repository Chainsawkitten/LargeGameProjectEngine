#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "linking.hpp"

namespace Video {
    class ShaderProgram;

    /// Draws debug primitives.
    class DebugDrawing {
        public:
        /// A debug drawing point.
        struct Point {
            /// World position.
            glm::vec3 position;

            /// Color.
            glm::vec3 color;

            /// Size.
            float size;

            /// Duration (in seconds).
            float duration;

            /// Whether to enable depth testing.
            bool depthTesting;
        };

        /// A debug drawing line.
        struct Line {
            /// Starting position of the line.
            glm::vec3 startPosition;

            /// End position of the line.
            glm::vec3 endPosition;

            /// Color.
            glm::vec3 color;

            /// Line width.
            float width;

            /// Duration (in seconds).
            float duration;

            /// Whether to enable depth testing.
            bool depthTesting;
        };

        /// A debug drawing cuboid.
        struct Cuboid {
            /// The minimum coordinates of the box.
            glm::vec3 minCoordinates;

            /// The maximum coordinates of the box.
            glm::vec3 maxCoordinates;

            /// Color.
            glm::vec3 color;

            /// Line width.
            float lineWidth;

            /// Duration (in seconds).
            float duration;

            /// Whether to enable depth testing.
            bool depthTesting;
        };

        /// A debug drawing plane.
        struct Plane {
            /// The center position of the plane.
            glm::vec3 position;

            /// The plane normal.
            glm::vec3 normal;

            /// Size.
            glm::vec2 size;

            /// Color.
            glm::vec3 color;

            /// Line width.
            float lineWidth;

            /// Duration (in seconds).
            float duration;

            /// Whether to enable depth testing.
            bool depthTesting;
        };

        /// A debug drawing sphere.
        struct Sphere {
            /// The center position of the sphere.
            glm::vec3 position;

            /// Radius.
            float radius;

            /// Color.
            glm::vec3 color;

            /// Line width.
            float lineWidth;

            /// Duration (in seconds).
            float duration;

            /// Whether to enable depth testing.
            bool depthTesting;
        };

        /// Create new debug primitive renderer.
        VIDEO_API DebugDrawing();

        /// Destructor.
        VIDEO_API ~DebugDrawing();

        /// Start rendering debug primitives.
        /**
             * Needs to be called before DrawPoint, DrawLine or DrawCuboid.
             * @param viewProjectionMatrix The camera's view projection matrix.
             */
        VIDEO_API void StartDebugDrawing(const glm::mat4& viewProjectionMatrix);

        /// Draw a point.
        /**
             * @param point The point to draw.
             */
        VIDEO_API void DrawPoint(const Point& point);

        /// Draw a line.
        /**
             * @param line The line to draw.
             */
        VIDEO_API void DrawLine(const Line& line);

        /// Draw a cuboid.
        /**
             * @param cuboid The cuboid to draw.
             */
        VIDEO_API void DrawCuboid(const Cuboid& cuboid);

        /// Draw a plane.
        /**
             * @param plane The plane to draw.
             */
        VIDEO_API void DrawPlane(const Plane& plane);

        /// Draw a sphere.
        /**
             * @param sphere The sphere to draw.
             */
        VIDEO_API void DrawSphere(const Sphere& sphere);

        /// Stop debug drawing.
        VIDEO_API void EndDebugDrawing();

        private:
        DebugDrawing(const DebugDrawing& other) = delete;

        static void CreateVertexArray(const glm::vec3* positions, unsigned int positionCount, GLuint& vertexBuffer, GLuint& vertexArray);
        void BindVertexArray(GLuint vertexArray);
        void CreateSphere(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail);

        Video::ShaderProgram* shaderProgram;

        GLuint pointVertexBuffer;
        GLuint pointVertexArray;

        GLuint lineVertexBuffer;
        GLuint lineVertexArray;

        GLuint cuboidVertexBuffer;
        GLuint cuboidVertexArray;

        GLuint planeVertexBuffer;
        GLuint planeVertexArray;

        GLuint sphereVertexBuffer;
        GLuint sphereVertexArray;
        unsigned int sphereVertexCount;

        GLuint boundVertexArray = 0;
    };
}
