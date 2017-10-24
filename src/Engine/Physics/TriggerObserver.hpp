#pragma once

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <functional>
#include "../linking.hpp"

class btRigidBody;

namespace Physics {
    /// Represents an object that listens to intersections against a trigger
    /// volume.
    /**
     * \attention Assumes an object of this type is passed to a trigger that is
     * always the same. If not, intersection phases may be erroneous.
     * \note Intended to be used only within the physics related engine classes.
     */
    class TriggerObserver : public btCollisionWorld::ContactResultCallback {
        public:
        /// The type of intersection this observer has to its trigger.
        enum class IntersectionPhase {
            /// Intersected during this frame.
            Enter,
            /// Intersection after previous enter.
            Retained,
            /// Stopped intersecting this frame.
            Leave,
            /// No intersection
            None,
        };

        public:
        /// Constructor.
        /**
             * @param body Rigid body listening to trigger.
             */
        ENGINE_API TriggerObserver(btRigidBody& body);

        /// Get the Bullet collision object of the observing body.
        /**
             * @return Bullet collision object.
             */
        ENGINE_API btCollisionObject* GetBulletCollisionObject();

        /// Get the intersection phase of the observer.
        /**
             * @return IntersectionPhase indicating the intersection phase
             * relative to the trigger volume.
             */
        ENGINE_API IntersectionPhase GetPhase() const;

        /// Determine new intersection phase after collision test has been
        /// applied.
        ENGINE_API void PostIntersectionTest();

        /// Set up a handler for when the observer begins intersecting its
        /// associated trigger volume.
        /**
             * @param handler Handler function to call.
             */
        ENGINE_API void OnEnter(const std::function<void()>& handler);

        /// Set up a handler for when the observer continues intersecting
        /// its associated trigger volume.
        /**
             * @param handler Handler function to call.
             */
        ENGINE_API void OnRetain(const std::function<void()>& handler);

        /// Set up a handler for when the observer stops intersecting its
        /// associated trigger volume.
        /**
             * @param handler Handler function to call.
             */
        ENGINE_API void OnLeave(const std::function<void()>& handler);

        private:
        /// Overridden from btCollisionWorld::ContactResultCallback for
        /// custom intersection handling.
        virtual btScalar addSingleResult(btManifoldPoint& cp,
            const btCollisionObjectWrapper* colObj0, int partId0, int index0,
            const btCollisionObjectWrapper* colObj1, int partId1, int index1) override;

        IntersectionPhase phase = IntersectionPhase::None;
        btRigidBody& rigidBody;
        // Control value to determine whether an intersection happened
        // during this frame. This is used to determine the new phase.
        bool didCallback = false;

        std::function<void()> enterHandler;
        std::function<void()> retainHandler;
        std::function<void()> leaveHandler;
    };
}
