#pragma once

#include <functional>
#include <map>
#include <memory>
#include "../linking.hpp"

class PhysicsManager;

namespace Physics {
    class Shape;
    class TriggerObserver;

    /// Represent a trigger that checks intersections of specific rigid bodies
    /// against itself.
    class Trigger {
        friend class ::PhysicsManager;

        public:
        /// Constructor.
        /**
             * @param transform The world transform of the trigger volume.
             */
        ENGINE_API Trigger(const btTransform& transform);

        private:
        // Get the wrapped Bullet collision object.
        btCollisionObject* GetCollisionObject() const;

        // Process observers against the trigger volume, passing the world
        // in which rigid bodies reside.
        void Process(btCollisionWorld& world);

        // Get access to a particular observer of the trigger to work with
        // it in a user-defined way. If the observer is not present, one
        // will be created.
        void ForObserver(btRigidBody* body, const std::function<void(TriggerObserver&)>& fun);

        void SetCollisionShape(std::shared_ptr<Shape> shape);

        private:
        btCollisionObject* trigger = nullptr;
        std::shared_ptr<Shape> shape = nullptr;
        std::vector<std::unique_ptr<TriggerObserver>> observers;
    };
}
