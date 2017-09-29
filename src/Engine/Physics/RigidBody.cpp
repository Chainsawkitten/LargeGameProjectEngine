#include "RigidBody.hpp"

#include <btBulletDynamicsCommon.h>
#include "GlmConversion.hpp"
#include "Shape.hpp"

namespace Physics {

    RigidBody::RigidBody(Physics::Shape* shape, float mass) : shape(shape) {
        // Motion states inform us of movement caused by physics so that we can
        // deal with those changes as needed.
        btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

        // Bullet treats zero mass as infinite, resulting in immovable objects.
        btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState, shape->GetShape(), btVector3(0, 0, 0));
        rigidBody = new btRigidBody(constructionInfo);
        rigidBody->setUserPointer(this);
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    RigidBody::~RigidBody() {
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
        return Physics::btToGlm(trans.getOrigin());
    }

    void RigidBody::Position(glm::vec3 const& pos) {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        trans.setOrigin(Physics::glmToBt(pos));
        rigidBody->setWorldTransform(trans);
    }

    btRigidBody* RigidBody::GetRigidBody() {
        return rigidBody;
    }

    Physics::Shape& RigidBody::GetShape() {
        return *shape;
    }

    void RigidBody::SetShape(Physics::Shape* shape) {
        delete this->shape;
        this->shape = shape;
        this->rigidBody->setCollisionShape(shape->GetShape());
    }

}
