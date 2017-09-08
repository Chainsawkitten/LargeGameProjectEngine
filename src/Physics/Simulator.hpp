#pragma once

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;
class btDefaultMotionState;
class btRigidBody;

namespace Physics {

    /// Simulates interactions among given objects.
    class Simulator {
        public:
            /// Constructor
            Simulator();

            /// Destructor
            ~Simulator();

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
