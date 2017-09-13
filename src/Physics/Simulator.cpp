#include "Simulator.hpp"

#include <btBulletDynamicsCommon.h>
#include "GlmConversion.hpp"
#include "RigidBody.hpp"

namespace Physics {

    Simulator::Simulator() {
        // The broadphase is used to quickly cull bodies that will not collide with
        // each other, normally by leveraging some simpler (and rough) test such as
        // bounding boxes.
        broadphase = new btDbvtBroadphase;

        // With the collision configuration one can configure collision detection
        // algorithms.
        collisionConfiguration = new btDefaultCollisionConfiguration;
        dispatcher = new btCollisionDispatcher(collisionConfiguration);

        // The solver makes objects interact by making use of gravity, collisions,
        // game logic supplied forces, and constraints.
        solver = new btSequentialImpulseConstraintSolver;

        // The dynamics world encompasses objects included in the simulation.
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

        // Y axis up
        dynamicsWorld->setGravity(btVector3(0, -9.82, 0));
    }

    Simulator::~Simulator() {
        delete dynamicsWorld;
        delete solver;
        delete dispatcher;
        delete collisionConfiguration;
        delete broadphase;
    }

    void Simulator::Simulate(float dt) {
        dynamicsWorld->stepSimulation(dt, 10);
    }

    void Simulator::Enable(RigidBody& body) {
        // Adding a rigid body to the world allows it to be simulated.
        dynamicsWorld->addRigidBody(body.rigidBody);
        body.simulator = this;
    }

    void Simulator::Disable(RigidBody& body) {
        dynamicsWorld->removeRigidBody(body.rigidBody);
        body.simulator = nullptr;
    }

    void Simulator::UseSphere(RigidBody& body, float radius) {
        // Collision shapes are only used to determine collisions and have no
        // concept of physical quantities such as mass or inertia. Note that a
        // shape may be reused for multiple bodies.
        btCollisionShape* shape = new btSphereShape(static_cast<btScalar>(radius));
        body.rigidBody->setCollisionShape(shape);
    }

    void Simulator::UsePlane(RigidBody& body, glm::vec3 normal, float planeConstant) {
        btCollisionShape* shape = new btStaticPlaneShape(glmToBt(normal), static_cast<btScalar>(planeConstant));
        body.rigidBody->setCollisionShape(shape);
    }

}
