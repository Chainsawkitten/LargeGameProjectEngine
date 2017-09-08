#include "Simulator.hpp"

#include <btBulletDynamicsCommon.h>

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

}
