#pragma once

#include <glm/glm.hpp>

namespace Video {
    class Texture2D;
    class ShaderProgram;
    namespace Geometry {
        class Geometry3D;
    }
    
    /// Render program to render an entity using default shader program.
    class StaticRenderProgram {
        public:
            /// Create new default render program.
            StaticRenderProgram();
            
            /// Destructor.
            ~StaticRenderProgram();
            
            /// Bind render program.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            void PreRender(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    
            /// Render mesh.
            /**
             * @param geometry The geometry to render.
             * @param textureAlbedo Albedo texture.
             * @param normalTexture Normal texture.
             * @param textureMetallic Metallic texture.
             * @param textureRoughness Roughness texture.
             * @param modelMatrix Model matrix.
             */
            void Render(Geometry::Geometry3D* geometry, const Video::Texture2D* textureAlbedo, const Video::Texture2D* normalTexture, const Video::Texture2D* textureMetallic, const Video::Texture2D* textureRoughness, const glm::mat4 modelMatrix) const;
            
        private:
            ShaderProgram* shaderProgram;
    
            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;
            glm::mat4 viewProjectionMatrix;
    };
}
