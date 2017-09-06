#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

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
            
            /// Create new debug primitive renderer.
            DebugDrawing();
            
            /// Destructor.
            ~DebugDrawing();
            
            /// Start rendering debug primitives.
            /**
             * Needs to be called before DrawPoint, DrawLine or DrawCuboid.
             * @param viewProjectionMatrix The camera's view projection matrix.
             */
            void StartDebugDrawing(const glm::mat4& viewProjectionMatrix);
            
            /// Draw a point.
            /**
             * @param point The point to draw.
             */
            void DrawPoint(const Point& point);
            
            /// Draw a line.
            /**
             * @param line The line to draw.
             */
            void DrawLine(const Line& line);
            
            /// Draw a cuboid.
            /**
             * @param cuboid The cuboid to draw.
             */
            void DrawCuboid(const Cuboid& cuboid);
            
            /// Stop debug drawing.
            void EndDebugDrawing();
            
        private:
            void BindVertexArray(GLuint vertexArray);
            
            Video::ShaderProgram* shaderProgram;
            
            GLuint pointVertexBuffer;
            GLuint pointVertexArray;
            
            GLuint lineVertexBuffer;
            GLuint lineVertexArray;
            
            GLuint cuboidVertexBuffer;
            GLuint cuboidVertexArray;
            
            GLuint boundVertexArray = 0;
    };
}
