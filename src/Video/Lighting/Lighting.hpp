#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Light.hpp"
#include <vector>

namespace Video {
    class ShaderProgram;
    namespace Geometry {
        class Rectangle;
    }
    
    /// Holds the frame buffers used for deferred rendering.
    class Lighting {
        public:
            /// Create new deferred lighting.
            /**
             * @param screenSize Size of the screen in pixels.
             * @param rectangle %Rectangle to use for rendering.
             */
            Lighting(const glm::vec2& screenSize, const Geometry::Rectangle* rectangle);
            
            /// Destructor.
            ~Lighting();
            
            /// Set as render target.
            void SetTarget();
            
            /// Clear all lights.
            void ClearLights();
            
            /// Add a light.
            /**
             * @param light The light to add.
             */
            void AddLight(const Light& light);
            
            /// Light the scene with the added lights.
            /**
             * @param inverseProjectionMatrix The camera's inverse projection matrix.
             */
            void Render(const glm::mat4& inverseProjectionMatrix);
            
        private:
            enum TEXTURE_TYPE {
                DIFFUSE,
                NORMAL,
                SPECULAR,
                GLOW,
                NUM_TEXTURES
            };
            
            static void AttachTexture(GLuint texture, unsigned int width, unsigned int height, GLenum attachment, GLint internalFormat);
            void BindForReading() const;
            
            GLuint textures[NUM_TEXTURES];
            
            GLuint frameBufferObject;
            GLuint depthHandle;
            
            const Geometry::Rectangle* rectangle;
            ShaderProgram* shaderProgram;
            
            std::vector<Light> lights;
            
            // Store light uniform locations so we don't have to get them every frame.
            static const unsigned int lightCount = 32U;
            struct {
                GLint position;
                GLint intensities;
                GLint attenuation;
                GLint ambientCoefficient;
                GLint coneAngle;
                GLint direction;
            } lightUniforms[lightCount];
    };
}
