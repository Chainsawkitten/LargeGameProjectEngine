#pragma once

#include <Video/Geometry/Geometry3D.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include "../linking.hpp"

namespace Geometry {
    /// A cube.
    class Cube : public Video::Geometry::Geometry3D {
        public:
        /// Create new cube.
        ENGINE_API Cube();

        /// Destructor.
        ENGINE_API ~Cube() final;

        /// Get geometry type.
        /**
             * @return Type.
             */
        ENGINE_API Type GetType() const override;

        protected:
        /// Generate vertex buffer.
        void GenerateVertexBuffer(GLuint& vertexBuffer);

        /// Generate vertex array.
        void GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray);

        private:
        std::vector<Video::Geometry::VertexType::StaticVertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<glm::vec3*> verticesPos;
    };
}
