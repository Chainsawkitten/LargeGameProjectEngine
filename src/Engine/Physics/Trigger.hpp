#pragma once

#include <functional>
#include <map>
#include <memory>

class PhysicsManager;

namespace Physics {
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
            Trigger(const btTransform& transform);

            /// Get the wrapped Bullet collision object.
            /**
             * @return The Bullet collision object.
             */
            btCollisionObject* GetCollisionObject() const;

            /// Process observers against the trigger volume.
            /**
             * @param world The world in which rigid bodies reside.
             */
            void Process(btCollisionWorld& world);

            /// Get access to a particular observer of the trigger to work with
            /// it in a user-defined way. If the observer is not present, one
            /// will be created.
            /**
             * @param body Observer to access.
             * @param fun Function that is called, passing the TriggerObserver.
             * accompanying the observer.
             */
            void ForObserver(btRigidBody* body, const std::function<void(TriggerObserver&)>& fun);

        private:
            void SetCollisionShape(btCollisionShape* shape) const;

        private:
            btCollisionObject* trigger = nullptr;
            std::vector<std::unique_ptr<TriggerObserver>> observers;
    };
}
