#pragma once

#include <GL/glew.h>
#include <Video/Culling/AxisAlignedBoundingBox.hpp>
#include <vector>

namespace Video {
    namespace Geometry {
        /// Renderable 3D geometry.
        class Geometry3D {
            public:
                /// Type of layout of vertex points.
                enum Type {
                    STATIC = 0, ///< Default3D vertex layout (Default3D.vert).
                    SKIN ///< Skinning vertex layout (Skinning.vert).
                };
                
                /// Destructor.
                virtual ~Geometry3D();
    
                /// Get the vertex array.
                /**
                 * @return The vertex array.
                 */
                GLuint GetVertexArray() const;
                
                /// Get number of indices.
                /**
                 * @return Index count.
                 */
                unsigned int GetIndexCount() const;
                
                /// Get the axis-aligned bounding box around the geometry.
                /**
                 * @return Local space axis-aligned bounding box around the geometry.
                 */
                const Video::AxisAlignedBoundingBox& GetAxisAlignedBoundingBox() const;
                
                /// Get geometry type.
                /**
                 * @return Type.
                 */
                virtual Type GetType() const = 0;
                
            protected:                
                /// Generate index buffer.
                /**
                 * @param indexData Pointer to array of indices.
                 * @param indexCount Number of indices.
                 * @param indexBuffer Index buffer.
                 */
                void GenerateIndexBuffer(unsigned int* indexData, unsigned int indexCount, GLuint& indexBuffer);
                
                /// Create local space axis-aligned bounding box around the geometry.
                /**
                 * @param positions Vector of vertex positions.
                 */
                void CreateAxisAlignedBoundingBox(const std::vector<glm::vec3*>& positions);

                /// Create local space axis-aligned bounding box around the geometry.
                /**
                 * @param dim Vector of vertex positions.
                 * @param origin Vector of vertex positions.
                 * @param minValues Vector of vertex positions.
                 * @param maxValues Vector of vertex positions.
                 */
                void CreateAxisAlignedBoundingBox(glm::vec3 dim, glm::vec3 origin, glm::vec3 minValues, glm::vec3 maxValues);
                
                /// Vertex buffer.
                GLuint vertexBuffer;
                
                /// Index buffer.
                GLuint indexBuffer;
                
                /// Vertex array.
                GLuint vertexArray;
                
            private:
                Video::AxisAlignedBoundingBox axisAlignedBoundingBox;
                unsigned int indexCount = 0;
        };
    }
}
