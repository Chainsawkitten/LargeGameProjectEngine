#include "RigidBody.hpp"

#include <btBulletDynamicsCommon.h>
#include "GlmConversion.hpp"
#include "Simulator.hpp"

namespace Physics {

    RigidBody::RigidBody() {
        // Motion states inform us of movement caused by physics so that we can
        // deal with those changes as needed.
        btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

        // Bullet treats zero mass as infinite, resulting in immovable objects.
        btRigidBody::btRigidBodyConstructionInfo constructionInfo(0, motionState, nullptr, btVector3(0, 0, 0));
        rigidBody = new btRigidBody(constructionInfo);
    }

    RigidBody::~RigidBody() {
        if (simulator) {
            simulator->Disable(*this);
        }

        delete rigidBody->getMotionState();
        delete rigidBody;
    }

    void RigidBody::Mass(float mass) {
        // Bullet provides a method on the shape that we can use to calculate
        // inertia.
        btVector3 inertia;
        rigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
        rigidBody->setMassProps(mass, inertia);
    }

    glm::vec3 RigidBody::Position() {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        return btToGlm(trans.getOrigin());
    }

    void RigidBody::Position(glm::vec3 const& pos) {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        trans.setOrigin(glmToBt(pos));
        rigidBody->setWorldTransform(trans);
    }

}