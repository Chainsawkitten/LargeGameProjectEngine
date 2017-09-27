#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Video {
    class ShaderProgram;
    class Texture;
    
    /// Renders particles.
    class ParticleRenderer {
        public:
            /// A particle in the particle system.
            struct Particle {
                /// Position.
                glm::vec3 worldPos;
                
                /// Size.
                glm::vec2 size;
                
                /// Life (in seconds).
                float life;
                
                /// Lifetime (in seconds).
                float lifetime;
                
                /// Initial velocity.
                glm::vec3 velocity;
                
                /// Start, mid and end of life alpha of particle.
                glm::vec3 alpha;
                
                /// Color of the particle.
                glm::vec3 color;
                
                /// Texture index (for the texture atlas, left to right, top to bottom indexing)
                float textureIndex;
            };
            
            /// Create new particle renderer.
            /**
             * @param maxParticleCount The maximum amount of particles in the buffer.
             */
            ParticleRenderer(unsigned int maxParticleCount);
            
            /// Destructor.
            ~ParticleRenderer();
            
            /// Set the particle buffer's contents.
            /**
             * @param count The number of particles.
             * @param particles The array of particles.
             */
            void SetBufferContents(unsigned int count, const Particle* particles);
            
            /// Render the particles.
            /**
             * @param textureAtlas The texture atlas containing particle textures.
             * @param textureAtlasRows The number of rows in the texture atlas.
             * @param cameraPosition The camera's position.
             * @param cameraUp The camera's up vector.
             * @param viewProjectionMatrix The camera's view projection matrix.
             */
            void Render(Texture* textureAtlas, unsigned int textureAtlasRows, const glm::vec3& cameraPosition, const glm::vec3& cameraUp, const glm::mat4& viewProjectionMatrix);
            
        private:
            ParticleRenderer(const ParticleRenderer & other) = delete;
            ShaderProgram* shaderProgram;
            
            // Vertex buffer.
            GLuint vertexBuffer = 0;
            GLuint vertexArray = 0;
            unsigned int vertexCount = 0;
            
            unsigned int particleCount = 0;
    };
}
