#include "PhysicsManager.hpp"

#include <btBulletDynamicsCommon.h>
#include <glm/gtx/quaternion.hpp>
#include "../Component/RigidBody.hpp"
#include "../Component/Shape.hpp"
#include "../Entity/Entity.hpp"
#include "../Physics/GlmConversion.hpp"
#include "../Physics/Shape.hpp"
#include "../Physics/Trigger.hpp"
#include "../Physics/TriggerObserver.hpp"
#include "../Util/Json.hpp"

PhysicsManager::PhysicsManager() {
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

    // Set the lockbox key we will use for lockboxes created in here.
    triggerLockBoxKey.reset(new Utility::LockBox<Physics::Trigger>::Key());
}

PhysicsManager::~PhysicsManager() {
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

void PhysicsManager::Update(float deltaTime) {
    for (auto rigidBodyComp : rigidBodyComponents.GetAll()) {
        if (rigidBodyComp->IsKilled() || !rigidBodyComp->entity->enabled) {
            continue;
        }

        rigidBodyComp->Position(rigidBodyComp->entity->position);
        dynamicsWorld->removeRigidBody(rigidBodyComp->GetBulletRigidBody());
        dynamicsWorld->addRigidBody(rigidBodyComp->GetBulletRigidBody());
        rigidBodyComp->GetBulletRigidBody()->setGravity(btVector3(0, 0, 0));
    }

    dynamicsWorld->stepSimulation(deltaTime, 10);

    for (auto trigger : triggers) {
        trigger->Process(*dynamicsWorld);
    }
}

void PhysicsManager::UpdateEntityTransforms() {
    for (auto rigidBodyComp : rigidBodyComponents.GetAll()) {
        if (rigidBodyComp->IsKilled() || !rigidBodyComp->entity->enabled)
            continue;

        Entity* entity = rigidBodyComp->entity;

        auto trans = rigidBodyComp->GetBulletRigidBody()->getWorldTransform();
        entity->position = Physics::btToGlm(trans.getOrigin());
        entity->rotation = glm::eulerAngles(Physics::btToGlm(trans.getRotation()));
    }
}

void PhysicsManager::OnTriggerEnter(Component::RigidBody* trigger, Component::RigidBody* object, std::function<void()> callback) {
    auto t = CreateTrigger(trigger);
    // Add the callback to the trigger observer
    t.Open(triggerLockBoxKey, [object, &callback](Physics::Trigger& trigger) {
        trigger.ForObserver(object->GetBulletRigidBody(), [&callback](Physics::TriggerObserver& observer) {
            observer.OnEnter(callback);
        });
    });
}

void PhysicsManager::OnTriggerRetain(Component::RigidBody* trigger, Component::RigidBody* object, std::function<void()> callback) {
    auto t = CreateTrigger(trigger);
    // Add the callback to the trigger observer
    t.Open(triggerLockBoxKey, [object, &callback](Physics::Trigger& trigger) {
        trigger.ForObserver(object->GetBulletRigidBody(), [&callback](::Physics::TriggerObserver& observer) {
            observer.OnRetain(callback);
        });
    });
}

void PhysicsManager::OnTriggerLeave(Component::RigidBody* trigger, Component::RigidBody* object, std::function<void()> callback) {
    auto t = CreateTrigger(trigger);
    // Add the callback to the trigger observer
    t.Open(triggerLockBoxKey, [object, &callback](Physics::Trigger& trigger) {
        trigger.ForObserver(object->GetBulletRigidBody(), [&callback](::Physics::TriggerObserver& observer) {
            observer.OnLeave(callback);
        });
    });
}

Component::RigidBody* PhysicsManager::CreateRigidBody(Entity* owner) {
    auto comp = rigidBodyComponents.Create();
    comp->entity = owner;

    comp->NewBulletRigidBody(1.0f);

    auto shapeComp = comp->entity->GetComponent<Component::Shape>();
    if (shapeComp) {
        comp->GetBulletRigidBody()->setCollisionShape(shapeComp->GetShape()->GetShape());
    }

    return comp;
}

Component::RigidBody* PhysicsManager::CreateRigidBody(Entity* owner, const Json::Value& node) {
    auto comp = rigidBodyComponents.Create();
    comp->entity = owner;

    auto mass = node.get("mass", 1.0f).asFloat();
    comp->NewBulletRigidBody(mass);

    auto shapeComp = comp->entity->GetComponent<Component::Shape>();
    if (shapeComp) {
        comp->GetBulletRigidBody()->setCollisionShape(shapeComp->GetShape()->GetShape());
    }

    return comp;
}

Component::Shape* PhysicsManager::CreateShape(Entity* owner) {
    auto comp = shapeComponents.Create();
    comp->entity = owner;

    auto shape = std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Sphere(1.0f)));
    comp->SetShape(shape);

    auto rigidBodyComp = comp->entity->GetComponent<Component::RigidBody>();
    if (rigidBodyComp) {
        rigidBodyComp->GetBulletRigidBody()->setCollisionShape(comp->GetShape()->GetShape());
    }

    return comp;
}

Component::Shape* PhysicsManager::CreateShape(Entity* owner, const Json::Value& node) {
    auto comp = shapeComponents.Create();
    comp->entity = owner;

    if (node.isMember("sphere")) {
        auto sphere = node.get("sphere", {});
        auto radius = sphere.get("radius", 1.0f).asFloat();
        auto shape = std::shared_ptr<::Physics::Shape>(new ::Physics::Shape(::Physics::Shape::Sphere(radius)));
        comp->SetShape(shape);
    } else if (node.isMember("plane")) {
        auto plane = node.get("plane", {});
        auto normal = Json::LoadVec3(plane.get("normal", {}));
        auto planeCoeff = plane.get("planeCoeff", 0.0f).asFloat();
        auto shape = std::shared_ptr<::Physics::Shape>(new ::Physics::Shape(::Physics::Shape::Plane(normal, planeCoeff)));
        comp->SetShape(shape);
    }

    auto rigidBodyComp = comp->entity->GetComponent<Component::RigidBody>();
    if (rigidBodyComp) {
        rigidBodyComp->GetBulletRigidBody()->setCollisionShape(comp->GetShape()->GetShape());
    }

    return comp;
}

Utility::LockBox<Physics::Trigger> PhysicsManager::CreateTrigger(Component::RigidBody* comp) {
    btTransform trans(btQuaternion(0, 0, 0, 1), ::Physics::glmToBt(comp->entity->position));
    Physics::Trigger* trigger = new Physics::Trigger(trans);
    auto shapeComp = comp->entity->GetComponent<Component::Shape>();
    trigger->SetCollisionShape(shapeComp ? shapeComp->GetShape() : nullptr);
    triggers.push_back(trigger);
    return Utility::LockBox<Physics::Trigger>(triggerLockBoxKey, trigger);
}

void PhysicsManager::SetShape(Component::Shape* comp, std::shared_ptr<::Physics::Shape> shape) {
    comp->SetShape(shape);
}

void PhysicsManager::SetMass(Component::RigidBody* comp, float mass) {
    // Setting mass is only valid with a shape because it also sets inertia.
    auto shapeComp = comp->entity->GetComponent<Component::Shape>();
    if (shapeComp)
        comp->Mass(mass);
}

const std::vector<Component::Shape*>& PhysicsManager::GetShapeComponents() const {
    return shapeComponents.GetAll();
}

void PhysicsManager::ClearKilledComponents() {
    rigidBodyComponents.ClearKilled(
        [this](Component::RigidBody* body) {
            dynamicsWorld->removeRigidBody(body->GetBulletRigidBody());
        });
    shapeComponents.ClearKilled();
}
