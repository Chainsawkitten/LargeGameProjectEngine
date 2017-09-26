#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Light.hpp"
#include <vector>

namespace Video {
    class FrameBuffer;
    class RenderSurface;
    class ShaderProgram;
    namespace Geometry {
        class Rectangle;
    }
    
    /// Holds the frame buffers used for deferred rendering.
    class Lighting {
        public:
            /// Create new deferred lighting.
            /**
             * @param rectangle %Rectangle to use for rendering.
             */
            Lighting(const Geometry::Rectangle* rectangle);
            
            /// Destructor.
            ~Lighting();
            
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
             * @param renderSurface %RenderSurface contaning textures.
             */
            void Render(const glm::mat4& inverseProjectionMatrix, RenderSurface* renderSurface);
            
        private:  
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
                GLint color;
            } lightUniforms[lightCount];
    };
}
