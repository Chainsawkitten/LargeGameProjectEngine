#include <btBulletDynamicsCommon.h>
#include "../Physics/GlmConversion.hpp"
#include "RigidBody.hpp"

namespace Component {
    RigidBody::~RigidBody() {
        Destroy();
    }

    Json::Value RigidBody::Save() const {
        Json::Value component;
        component["mass"] = mass;
        return component;
    }

    btRigidBody* RigidBody::GetBulletRigidBody() {
        return rigidBody;
    }

    void RigidBody::NewBulletRigidBody(float mass) {
        Destroy();

        this->mass = mass;

        // Motion states inform us of movement caused by physics so that we can
        // deal with those changes as needed.
        btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

        // Bullet treats zero mass as infinite, resulting in immovable objects.
        btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState, nullptr, btVector3(0, 0, 0));
        rigidBody = new btRigidBody(constructionInfo);
        rigidBody->setUserPointer(this);
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    void RigidBody::Destroy() {
        if (rigidBody) {
            delete rigidBody->getMotionState();
            delete rigidBody;
        }
    }

    glm::vec3 RigidBody::Position() const {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        return Physics::btToGlm(trans.getOrigin());
    }

    void RigidBody::Position(const glm::vec3& pos) {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        trans.setOrigin(Physics::glmToBt(pos));
        rigidBody->setWorldTransform(trans);
    }

    void RigidBody::Mass(float mass) {
        // Bullet provides a method on the shape that we can use to calculate
        // inertia.
        btVector3 inertia;
        rigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
        rigidBody->setMassProps(mass, inertia);
    }
}
