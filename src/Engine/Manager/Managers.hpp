#pragma once

#include <typeinfo>

class ResourceManager;
class RenderManager;
class ParticleManager;
class PhysicsManager;
class SoundManager;
class ScriptManager;
class DebugDrawingManager;
class ProfilingManager;

class World;

namespace Component {
    class SuperComponent;
}

/// Singleton class that holds all subsystems.
class Hub {

    friend Hub& Managers();
    
    public:

        /// The resource manager instance.
        ResourceManager* resourceManager;
        
        /// The render manager instance.
        RenderManager* renderManager;
        
        /// The particle manager instance.
        ParticleManager* particleManager;
        
        /// The physics manager instance.
        PhysicsManager* physicsManager;
        
        /// The sound manager instance.
        SoundManager* soundManager;
        
        /// The script manager instance.
        ScriptManager* scriptManager;
        
        /// The debug drawing manager instance.
        DebugDrawingManager* debugDrawingManager;
        
        /// The profiling manager instance.
        ProfilingManager* profilingManager;
        
        /// Initialize all subsystems.
        void StartUp();
        
        /// Deinitialize all subsystems.
        void ShutDown();

        /// Adds a component to the correct manager.
        /**
        * @param component The component to add.
        * @param componentType The type of the component to add.
        */
        void AddComponent(World* world, Component::SuperComponent* component, const std::type_info* componentType);

        /// Clears the components of a certain world in all managers.
        void ClearComponents(World* world);

        /// Clears all the components of all managers.
        void ClearAllComponents();

        /// Clears the killed components of all managers.
        void ClearKilledComponents(World* world);
        
    private:
        Hub();
        Hub(const Hub&) = delete;
        void operator=(const Hub&) = delete;

        bool shutdown = false;

};

/// Get a hub containing all the subsystems.
/**
 * @return A hub containing all the subsystems.
 */
Hub& Managers();
