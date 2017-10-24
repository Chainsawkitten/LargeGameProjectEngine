#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <Utility/LockBox.hpp>
#include <vector>
#include "../Entity/ComponentContainer.hpp"
#include "../linking.hpp"

namespace Component {
    class RigidBody;
    class Shape;
}

namespace Physics {
    class RigidBody;
    class Shape;
    class Trigger;
}

namespace Json {
    class Value;
}

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class Entity;

/// Updates the physics of the world.
class PhysicsManager {
    friend class Hub;

    public:
    /// Moves entities and updates the physics component.
    /**
         * @param deltaTime Time since last frame (in seconds).
         */
    ENGINE_API void Update(float deltaTime);

    /// Update transforms of entities according to positions of physics
    /// components.
    ENGINE_API void UpdateEntityTransforms();

    /// Set up listener for when |object| has entered |trigger|.
    /**
         * @param trigger What trigger to check against.
         * @param object Body that is to enter the trigger volume.
         * @param callback Function to call when resolving event.
         */
    ENGINE_API void OnTriggerEnter(Component::RigidBody* trigger, Component::RigidBody* object, std::function<void()> callback);

    /// Set up listener for when |object| is intersecting |trigger|.
    /**
         * @param trigger What trigger to check against.
         * @param object Body that is to cause trigger to fire.
         * @param callback Function to call when resolving event.
         */
    ENGINE_API void OnTriggerRetain(Component::RigidBody* trigger, Component::RigidBody* object, std::function<void()> callback);

    /// Set up listener for when |object| has left |trigger|.
    /**
         * @param trigger What trigger to check against.
         * @param object Body that is to cause trigger to fire.
         * @param callback Function to call when resolving event.
         */
    ENGINE_API void OnTriggerLeave(Component::RigidBody* trigger, Component::RigidBody* object, std::function<void()> callback);

    /// Create rigid body component.
    /**
         * @param owner The %Entity that will own the component.
         * @return The created component.
         */
    ENGINE_API Component::RigidBody* CreateRigidBody(Entity* owner);

    /// Create rigid body component.
    /**
         * @param owner The %Entity that will own the component.
         * @param node Json node from which to load component definition.
         * @return The created component.
         */
    ENGINE_API Component::RigidBody* CreateRigidBody(Entity* owner, const Json::Value& node);

    /// Create a component that represents a physical shape.
    /**
         * @param owner The %Entity that will own the component.
         * @return The created component.
         */
    ENGINE_API Component::Shape* CreateShape(Entity* owner);

    /// Create a component that represents a physical shape.
    /**
         * @param owner The %Entity that will own the component.
         * @param node Json node from which to load component definition.
         * @return The created component.
         */
    ENGINE_API Component::Shape* CreateShape(Entity* owner, const Json::Value& node);

    /// Create a trigger volume that can be used to check intersection
    /// events against physics bodies.
    /**
         * @param comp Rigid body that represents the volume. This is intended
         * to be changed to a pure shape in the future.
         * @return A reference to the internal trigger.
         */
    ENGINE_API Utility::LockBox<Physics::Trigger> CreateTrigger(Component::RigidBody* comp);

    /// Set the shape of a given Component::Shape component.
    /**
         * @param comp The component on which to set the shape.
         * @param A Physics::Shape object that holds the shape definition.
         */
    ENGINE_API void SetShape(Component::Shape* comp, std::shared_ptr<::Physics::Shape> shape);

    /// Set the mass of a Component::RigidBody component.
    /**
         * @param comp The component on which to set mass.
         * @param mass Mass in kilograms.
         */
    ENGINE_API void SetMass(Component::RigidBody* comp, float mass);

    /// Get all shape components.
    /**
         * @return All shape components.
         */
    ENGINE_API const std::vector<Component::Shape*>& GetShapeComponents() const;

    /// Remove all killed components.
    ENGINE_API void ClearKilledComponents();

    private:
    PhysicsManager();
    ~PhysicsManager();
    PhysicsManager(PhysicsManager const&) = delete;
    void operator=(PhysicsManager const&) = delete;

    glm::vec3 gravity = glm::vec3(0.f, -9.82f, 0.f);

    ComponentContainer<Component::RigidBody> rigidBodyComponents;
    ComponentContainer<Component::Shape> shapeComponents;

    btBroadphaseInterface* broadphase = nullptr;
    btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
    btCollisionDispatcher* dispatcher = nullptr;
    btSequentialImpulseConstraintSolver* solver = nullptr;
    btDiscreteDynamicsWorld* dynamicsWorld = nullptr;

    std::shared_ptr<Utility::LockBox<Physics::Trigger>::Key> triggerLockBoxKey;
    std::vector<::Physics::Trigger*> triggers;
};
