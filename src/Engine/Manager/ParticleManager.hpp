#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <random>

class Entity;
class World;
namespace Video {
    class ParticleRenderer;
}
class Texture2D;
namespace Component {
    class ParticleEmitter;
}

/// Handles particles.
class ParticleManager {
    friend class Hub;
    
    public:
        /// Get the maximum amount of particles.
        /**
         * @return Maximum amount of particles.
         */
        unsigned int GetMaxParticleCount() const;
        
        /// Update all the system's particles, spawn new particles etc.
        /**
         * @param world World to update.
         * @param time Time since last frame (in seconds).
         * @param preview Whether to only update particle emitters that are being previewed.
         */
        void Update(World& world, float time, bool preview = false);
        
        /// Update particle buffer.
        /**
         * Needs to be called before rendering (but only once a frame).
         * @param world The world to render.
         */
        void UpdateBuffer(World& world);

        /// Render the particles in a world.
        /**
         * @param world %World containing particles to render.
         * @param camera Camera through which to render.
         */
        void Render(World& world, const Entity* camera);
        
        /// Get the texture atlas.
        /**
         * @return The particle texture atlas.
         */
        const Texture2D* GetTextureAtlas() const;
        
        /// Get the number of rows in the texture atlas.
        /**
         * @return The number of rows in the texture atlas.
         */
        int GetTextureAtlasRows() const;
        
    private:
        ParticleManager();
        ~ParticleManager();
        ParticleManager(ParticleManager const&) = delete;
        void operator=(ParticleManager const&) = delete;
        
        // Decide where the emitter should emit before rendering.
        void EmitParticle(World& world, Component::ParticleEmitter* emitter);
        
        // Emit a particle at the given position.
        void EmitParticle(World& world, const glm::vec3& position, Component::ParticleEmitter* emitter);
        
        unsigned int maxParticleCount = 10000;
        
        std::random_device randomDevice;
        std::mt19937 randomEngine;
        
        Video::ParticleRenderer* particleRenderer;

        // The number of rows in the texture atlas.
        int textureAtlasRowNumber = 4;

        // Texture atlas containing the particle textures.
        Texture2D* textureAtlas;
};
