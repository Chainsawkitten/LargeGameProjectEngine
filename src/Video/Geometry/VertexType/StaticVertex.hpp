#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../../linking.hpp"

namespace Video {
    namespace Geometry {
        namespace VertexType {
            /// Vertex type used for static meshes.
            struct StaticVertex {
                /// Position.
                glm::vec3 position;

                /// %Texture coordinate.
                glm::vec2 textureCoordinate;

                /// Normal.
                glm::vec3 normal;

                /// Tangent vector.
                glm::vec3 tangent;

                /// Generate vertex buffer
                /**
                 * @param vertices Pointer to vertex array.
                 * @param vertexCount Number of vertices.
                 * @return Vertex buffer.
                 */
                VIDEO_API static const GLuint GenerateVertexBuffer(StaticVertex* vertices, unsigned int vertexCount);

                /// Generate vertex buffer
                /**
                 * @param vertexBuffer The vertex buffer.
                 * @param indexBuffer The index buffer.
                 * @return Vertex array.
                 */
                VIDEO_API static const GLuint GenerateVertexArray(GLuint vertexBuffer, GLuint indexBuffer);
            };
        }
    }
}
