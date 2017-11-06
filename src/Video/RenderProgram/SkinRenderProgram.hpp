#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Video {
    class Texture2D;
    class ShaderProgram;
    class StorageBuffer;
    namespace Geometry {
        class Geometry3D;
    }
    
    /// Render program to render an entity using skin shader program.
    class SkinRenderProgram {
        public:
            /// Create new default render program.
            SkinRenderProgram();
            
            /// Destructor.
            ~SkinRenderProgram();
            
            /// Bind render program.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param lightBuffer Storage buffer for lights.
             * @param lightCount Number of lights.
             */
            void PreRender(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const StorageBuffer* lightBuffer, unsigned int lightCount);
    
            /// Render skinned geometry.
            /**
             * @param geometry The geometry to render.
             * @param textureAlbedo Albedo texture.
             * @param normalTexture Normal texture.
             * @param textureMetallic Metallic texture.
             * @param textureRoughness Roughness texture.
             * @param modelMatrix Model matrix.
             * @param bones Transformations of skeleton.
             */
            void Render(const Video::Geometry::Geometry3D* geometry, const Video::Texture2D* textureAlbedo, const Video::Texture2D* normalTexture, const Video::Texture2D* textureMetallic, const Video::Texture2D* textureRoughness, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones) const;
            
        private:
            SkinRenderProgram(const SkinRenderProgram & other) = delete;
            ShaderProgram* shaderProgram;
    
            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;
            glm::mat4 viewProjectionMatrix;
    };
}
