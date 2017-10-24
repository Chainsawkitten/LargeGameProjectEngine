#pragma once

#include <vector>
#include <map>
#include <typeinfo>
#include <Video/ParticleRenderer.hpp>
#include "../linking.hpp"

class Entity;
namespace Component {
    class SuperComponent;
}
namespace Json {
    class Value;
}

/// The game world containing all entities.
class World {
    friend class Entity;

    public:
    /// Create a new world.
    ENGINE_API World();

    /// Destructor.
    ENGINE_API ~World();

    /// Create a new entity in the world.
    /**
         * @param name Name of the entity to create.
         * @return The new entity.
         */
    ENGINE_API Entity* CreateEntity(const std::string& name = "");

    /// Get all the entities in the world.
    /**
         * @return The entities in the world.
         */
    ENGINE_API const std::vector<Entity*>& GetEntities() const;

    /// Create root entity.
    ENGINE_API void CreateRoot();

    /// Get the root entity.
    /**
         * @return The root entity.
         */
    ENGINE_API Entity* GetRoot() const;

    /// Register an entity to receive update events.
    /**
         * @param entity %Entity to register.
         */
    ENGINE_API void RegisterUpdate(Entity* entity);

    /// Get all entities that are registered to receive update events.
    /**
         * @return A list of all entities registered to receive update events.
         */
    ENGINE_API const std::vector<Entity*>& GetUpdateEntities() const;

    /// Clear the world of all entities.
    ENGINE_API void Clear();

    /// Removes all killed entities and components in the world.
    ENGINE_API void ClearKilled();

    /// Get all the particles in the world.
    /**
         * @return Array of all the particles in the world.
         */
    ENGINE_API Video::ParticleRenderer::Particle* GetParticles() const;

    /// Get the number of particles in the world.
    /**
         * @return The number of particles in the world.
         */
    ENGINE_API unsigned int GetParticleCount() const;

    /// Set the number of particles in the world.
    /**
         * @param particleCount The number of particles in the world.
         */
    ENGINE_API void SetParticleCount(unsigned int particleCount);

    /// Save the world to file.
    /**
         * @param filename The name of the file.
         */
    ENGINE_API void Save(const std::string& filename) const;

    /// Get a json file representing the root.
    /**
         * @return The json file representing the root.
         */
    ENGINE_API Json::Value GetSaveJson() const;

    /// Load the world from file.
    /**
         * @param filename The name of the file.
         */
    ENGINE_API void Load(const std::string& filename);

    /// Load the world from a json.
    /**
         * @param node The json containing a scene to load.
         */
    ENGINE_API void Load(const Json::Value& node);

    private:
    // Copy constructor.
    World(World& world) = delete;

    // List of all entities in this world.
    std::vector<Entity*> entities;
    Entity* root = nullptr;

    // All particles in the world.
    Video::ParticleRenderer::Particle* particles;
    unsigned int particleCount = 0;

    // Entities registered for update event.
    std::vector<Entity*> updateEntities;
};
