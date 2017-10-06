#pragma once

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>

class btRigidBody;

namespace Physics {
    /// Represents an object that listens to intersections against a trigger
    /// volume.
    /**
     * \attention Assumes an object of this type is passed to a trigger that is
     * always the same. If not, intersection phases may be erroneous.
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
            TriggerObserver(btRigidBody& body);

            /// Get the Bullet collision object of the observing body.
            /**
             * @return Bullet collision object.
             */
            btCollisionObject* GetBulletCollisionObject();

            /// Get the intersection phase of the observer.
            /**
             * @return IntersectionPhase indicating the intersection phase
             * relative to the trigger volume.
             */
            IntersectionPhase GetPhase() const;

            /// Determine new intersection phase after collision test has been
            /// applied.
            void PostIntersectionTest();

        private:
            /// Overridden from btCollisionWorld::ContactResultCallback for
            /// custom intersection handling.
            virtual btScalar addSingleResult(btManifoldPoint& cp,
                const btCollisionObjectWrapper* colObj0, int partId0, int index0,
                const btCollisionObjectWrapper* colObj1, int partId1, int index1) override;

        private:
            IntersectionPhase phase = IntersectionPhase::None;
            btRigidBody& rigidBody;
            // Control value to determine whether an intersection happened
            // during this frame. This is used to determine the new phase.
            bool didCallback = false;
    };
}
