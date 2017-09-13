#pragma once

#include <LinearMath/btVector3.h>
#include <glm/glm.hpp>
#include <memory>

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;
class btDefaultMotionState;
class btRigidBody;

namespace Physics {

    class RigidBody;

    /// Simulates interactions among given objects.
    class Simulator {
        public:
            /// Constructor
            Simulator();

            /// Destructor
            ~Simulator();

            /// Adds a rigid body to the simulation world.
            /**
             * @param body The rigid body to simulate.
             */
            void Enable(RigidBody& body);

            /// Removes a rigid body from the simulation world.
            /**
             * @param body The rigid body to remove.
             */
            void Disable(RigidBody& body);

            /// Alters a rigid body to have a sphere shape.
            /**
             * @param body The rigid body to change shape of.
             * @param radius Radius in meters of the sphere.
             */
            void UseSphere(RigidBody& body, float radius);

            /// Alters a rigid body to have a plane shape.
            /**
             * @param body The rigid body to change shape of.
             * @param normal Plane normal.
             * @param planeConstant Plane constant.
             */
            void UsePlane(RigidBody& body, glm::vec3 normal, float planeConstant);

        protected:
            /// Execute a frame
            /**
             * @param dt Elapsed time in seconds since last simulation frame.
             */
            void Simulate(float dt);

        private:
            btBroadphaseInterface* broadphase = nullptr;
            btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
            btCollisionDispatcher* dispatcher = nullptr;
            btSequentialImpulseConstraintSolver* solver = nullptr;
            btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
    };

}
