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
             * @param diffuseTexture Diffuse texture.
             * @param normalTexture Normal map.
             * @param specularTexture Specular map.
             * @param glowTexture Glow texture.
             * @param modelMatrix Model matrix.
             */
            void Render(Geometry::Geometry3D* geometry, const Texture2D* diffuseTexture, const Texture2D* normalTexture, const Texture2D* specularTexture, const Texture2D* glowTexture, const glm::mat4 modelMatrix) const;
            
        private:
            ShaderProgram* shaderProgram;
    
            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;
            glm::mat4 viewProjectionMatrix;
    };
}
