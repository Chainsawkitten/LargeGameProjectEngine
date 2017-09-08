#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Video {
    class Texture2D;
    class ShaderProgram;
    namespace Geometry {
        class Geometry3D;
    }
    
    /// Render program to render an entity using skin shader program.
    class SkinRenderProgram {
        public:
            /// Create new default render program.
            /**
             * @param shaderProgram A GLSL shader program.
             */
            SkinRenderProgram();
            
            /// Destructor.
            ~SkinRenderProgram();
            
            /// Bind render program.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            void PreRender(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    
            /// Render skinned geometry.
            /**
             * @param geometry The geometry to render.
             * @param diffuseTexture Diffuse texture.
             * @param normalTexture Normal map.
             * @param specularTexture Specular map.
             * @param glowTexture Glow texture.
             * @param modelMatrix Model matrix.
             * @param bones Transformations of skeleton.
             * @param bonesIT Inverse transpose transformations of skeleton.
             */
            void Render(const Video::Geometry::Geometry3D* geometry, const Video::Texture2D* diffuseTexture, const Video::Texture2D* normalTexture, const Video::Texture2D* specularTexture, const Video::Texture2D* glowTexture, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones, const std::vector<glm::mat3>& bonesIT) const;
            
        private:
            ShaderProgram* shaderProgram;
    
            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;
            glm::mat4 viewProjectionMatrix;
    };
}
