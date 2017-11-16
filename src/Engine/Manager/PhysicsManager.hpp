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
        ENGINE_API void OnTriggerEnter(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object, std::function<void()> callback);

        /// Stop listening for when |object| has entered |trigger|.
        /**
         * @param trigger What trigger to stop listening on.
         * @param object Body that is to be forgotten.
         */
        ENGINE_API void ForgetTriggerEnter(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object);

        /// Set up listener for when |object| is intersecting |trigger|.
        /**
         * @param trigger What trigger to check against.
         * @param object Body that is to cause trigger to fire.
         * @param callback Function to call when resolving event.
         */
        ENGINE_API void OnTriggerRetain(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object, std::function<void()> callback);

        /// Stop listening for when |object| is intersecting |trigger|.
        /**
         * @param trigger What trigger to stop listening on.
         * @param object Body that is to be forgotten.
         */
        ENGINE_API void ForgetTriggerRetain(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object);

        /// Set up listener for when |object| has left |trigger|.
        /**
         * @param trigger What trigger to check against.
         * @param object Body that is to cause trigger to fire.
         * @param callback Function to call when resolving event.
         */
        ENGINE_API void OnTriggerLeave(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object, std::function<void()> callback);

        /// Stop listening for when |object| has left |trigger|.
        /**
         * @param trigger What trigger to stop listening on.
         * @param object Body that is to be forgotten.
         */
        ENGINE_API void ForgetTriggerLeave(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object);

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
         * @param shape Shape of the trigger volume.
         * @return A reference to the internal trigger.
         */
        ENGINE_API Utility::LockBox<Physics::Trigger> CreateTrigger(std::shared_ptr<Physics::Shape> shape);

        /// Inform the physics manager that a given trigger volume will no
        /// longer be used so that it can be freed from memory.
        /**
         * @param trigger Trigger volume to release.
         */
        ENGINE_API void ReleaseTriggerVolume(Utility::LockBox<Physics::Trigger>&& trigger);

        /// Set the position of a trigger volume.
        /**
         * @param trigger Volume to reposition.
         * @param position New position in world space.
         */
        ENGINE_API void SetPosition(Utility::LockBox<Physics::Trigger> trigger, const glm::vec3& position);

        /// Set the shape of a given Component::Shape component.
        /**
         * @param comp The component on which to set the shape.
         * @param shape A Physics::Shape object that holds the shape definition.
         */
        ENGINE_API void SetShape(Component::Shape* comp, std::shared_ptr<::Physics::Shape> shape);

        /// Get the mass of a rigid body component.
        /**
         * @param comp Rigid body component to get mass of.
         * @return Mass in kilograms.
         */
        ENGINE_API float GetMass(Component::RigidBody* comp);

        /// Set the volume shape of a trigger.
        /**
         * @param trigger Trigger to alter shape of.
         * @param shape Shape definition.
         */
        ENGINE_API void SetShape(Utility::LockBox<Physics::Trigger> trigger, std::shared_ptr<Physics::Shape> shape);

        /// Set the mass of a Component::RigidBody component.
        /**
         * @param comp The component on which to set mass.
         * @param mass Mass in kilograms.
         */
        ENGINE_API void SetMass(Component::RigidBody* comp, float mass);

        /// Set the friction coefficient of a Component::RigidBody component.
        /**
         * @param comp Rigid body to alter.
         * @param friction Friction coefficient.
         */
        ENGINE_API void SetFriction(Component::RigidBody* comp, float friction);

        /// Set the rolling friction coefficient of a Component::RigidBody
        /// component.
        /**
         * @param comp Rigid body to alter.
         * @param friction Friction coefficient.
         */
        ENGINE_API void SetRollingFriction(Component::RigidBody* comp, float friction);

        /// Set the spinning friction coefficient of a Component::RigidBody
        /// component.
        /**
         * @param comp Rigid body to alter.
         * @param friction Friction coefficient.
         */
        ENGINE_API void SetSpinningFriction(Component::RigidBody* comp, float friction);

        /// Set the restitution (bounciness) of a Component::RigidBody component.
        /**
         * @param comp Rigid body to alter.
         * @param cor Coefficient of restitution.
         */
        ENGINE_API void SetRestitution(Component::RigidBody* comp, float cor);

        /// Set the linear damping factor of a Component::RigidBody component.
        /**
         * @param comp Rigid body to alter.
         * @param damping Linear damping.
         */
        ENGINE_API void SetLinearDamping(Component::RigidBody* comp, float damping);

        /// Set the angular damping factor of a Component::RigidBody component.
        /**
         * @param comp Rigid body to alter.
         * @param damping Angular damping.
         */
        ENGINE_API void SetAngularDamping(Component::RigidBody* comp, float damping);

        /// Turn a rigid body into a kinematic object, putting movement in the
        /// control of the programmer.
        /**
         * @param comp Rigid body to make kinematic.
         */
        ENGINE_API void MakeKinematic(Component::RigidBody* comp);

        /// Turn a rigid body into a dynamic object.
        /**
         * @param comp Rigid body to make dynamic.
         */
        ENGINE_API void MakeDynamic(Component::RigidBody* comp);

        /// Forces a dynamic rigid body to synchronize its transform with that
        /// of its owning entity during the next simulation iteration.
        /**
         * @param comp Rigid body to synchronize.
         */
        ENGINE_API void ForceTransformSync(Component::RigidBody* comp);

        /// Halts movement of a kinematic rigid body.
        /**
         * @param comp Rigid body to halt.
         */
        ENGINE_API void HaltMovement(Component::RigidBody* comp);

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
