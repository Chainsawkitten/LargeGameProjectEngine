#include "Trigger.hpp"

#include <btBulletDynamicsCommon.h>
#include "../Component/RigidBody.hpp"
#include "Shape.hpp"

namespace Physics {

    Trigger::Trigger(const btTransform& transform) : btCollisionWorld::ContactResultCallback() {
        trigger = new btCollisionObject();
        trigger->setWorldTransform(transform);
    }

    // Called with each contact for our own processing. This is where we can
    // test whether the given listeners are intersecting the trigger volume.
    btScalar Trigger::addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0, int partId0, int index0,
        const btCollisionObjectWrapper* colObj1, int partId1, int index1) {

        auto otherCollisionObject = (colObj0->getCollisionObject() == trigger) ? colObj1 : colObj0;
        auto body = static_cast<btRigidBody*>(const_cast<btCollisionObject*>(otherCollisionObject->getCollisionObject()));
        
        // Call the corresponding callback
        observers[body]();
        return 0; // Was a planned purpose, but is not used.
    }

    btCollisionObject* Trigger::GetCollisionObject() {
        return trigger;
    }

    void Trigger::Process(btCollisionWorld& world) {
        for (auto& observerPair : observers) {
            world.contactPairTest(trigger, observerPair.first, *this);
        }
    }

    void Trigger::OnEnter(Component::RigidBody* body, std::function<void()> observer) {
        observers[body->GetBulletRigidBody()] = observer;
    }

    void Trigger::SetCollisionShape(btCollisionShape* shape) {
        trigger->setCollisionShape(shape);
    }

}
