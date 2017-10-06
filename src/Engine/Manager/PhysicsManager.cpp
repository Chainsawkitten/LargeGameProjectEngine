#include "PhysicsManager.hpp"

#include <btBulletDynamicsCommon.h>
#include <glm/gtx/quaternion.hpp>
#include "../Component/Physics.hpp"
#include "../Component/RigidBody.hpp"
#include "../Component/Shape.hpp"
#include "../Entity/Entity.hpp"
#include "../Physics/GlmConversion.hpp"
#include "../Physics/ITrigger.hpp"
#include "../Physics/Shape.hpp"
#include "../Physics/Trigger.hpp"
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
}

PhysicsManager::~PhysicsManager() {
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

void PhysicsManager::Update(float deltaTime) {
    for (Component::Physics* physicsComp : physicsComponents.GetAll()) {
        if (physicsComp->IsKilled() || !physicsComp->entity->enabled)
            continue;
        
        Entity* entity = physicsComp->entity;
        // --- Velocity ---
        // Add acceleration.
        physicsComp->velocity += (physicsComp->acceleration + this->gravity * physicsComp->gravityFactor) * deltaTime;
        
        // Add retardation.
        physicsComp->velocity -= physicsComp->velocity * physicsComp->velocityDragFactor * deltaTime;
        if (glm::length(physicsComp->velocity) > 0.01f) {
            // Cap velocity.
            if (glm::length(physicsComp->velocity) > physicsComp->maxVelocity)
                physicsComp->velocity = glm::length(physicsComp->maxVelocity) / glm::length(physicsComp->velocity) * physicsComp->velocity;
        } else {
            physicsComp->velocity = glm::vec3(0.f, 0.f, 0.f);
        }
        
        // Update position.
        entity->position += physicsComp->velocity * deltaTime;
        
        // --- Angular Velocity ---
        // Add angular acceleration.
        physicsComp->angularVelocity += physicsComp->angularAcceleration * physicsComp->momentOfInertia * deltaTime;
        
        // Add drag.
        physicsComp->angularVelocity -= physicsComp->angularVelocity * physicsComp->angularDragFactor * deltaTime;
        if (glm::length(physicsComp->angularVelocity) > 0.01f) {
            // Cap angular velocity.
            if (glm::length(physicsComp->angularAcceleration) > physicsComp->maxAngularVelocity)
                physicsComp->angularAcceleration = glm::length(physicsComp->maxAngularVelocity) / glm::length(physicsComp->angularAcceleration) * physicsComp->angularAcceleration;
        } else {
            physicsComp->angularAcceleration = glm::vec3(0.f, 0.f, 0.f);
        }
        
        // Update rotation.
        entity->rotation += physicsComp->angularVelocity * 360.f * deltaTime;
    }

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

void PhysicsManager::OnTriggerEnter(Component::Physics* triggerBody, Component::Physics* object, std::function<void()> callback) {
    auto trigger = MakeTrigger(triggerBody);
    auto rigidBodyComp = object->entity->GetComponent<Component::RigidBody>();
    assert(rigidBodyComp); // For now
    trigger->OnEnter(rigidBodyComp, callback);
}

Physics::Trigger* PhysicsManager::MakeTrigger(Component::Physics* comp) {
    auto rigidBodyComp = comp->entity->GetComponent<Component::RigidBody>();
    assert(rigidBodyComp); // for now
    Physics::Trigger* trigger = new Physics::Trigger(rigidBodyComp->GetBulletRigidBody()->getWorldTransform());
    auto shapeComp = comp->entity->GetComponent<Component::Shape>();
    trigger->SetCollisionShape(shapeComp ? shapeComp->GetShape().GetShape() : nullptr);
    triggers.push_back(trigger);
    return trigger;
}

Component::Physics* PhysicsManager::CreatePhysics(Entity* owner) {
    auto comp = physicsComponents.Create();
    comp->entity = owner;

    auto rigidBodyComp = comp->entity->GetComponent<Component::RigidBody>();
    auto shapeComp = comp->entity->GetComponent<Component::Shape>();
    if (rigidBodyComp && shapeComp) {
        rigidBodyComp->GetBulletRigidBody()->setCollisionShape(shapeComp->GetShape().GetShape());
    }

    return comp;
}

Component::Physics* PhysicsManager::CreatePhysics(Entity* owner, const Json::Value& node) {
    Component::Physics* physics = physicsComponents.Create();
    physics->entity = owner;
    
    // Load values from Json node.
    physics->velocity = Json::LoadVec3(node["velocity"]);
    physics->maxVelocity = node.get("maxVelocity", 20.f).asFloat();
    physics->angularVelocity = Json::LoadVec3(node["angularVelocity"]);
    physics->maxAngularVelocity = node.get("maxAngularVelocity", 2.f).asFloat();
    physics->acceleration = Json::LoadVec3(node["acceleration"]);
    physics->angularAcceleration = Json::LoadVec3(node["angularAcceleration"]);
    physics->velocityDragFactor = node.get("velocityDragFactor", 1.f).asFloat();
    physics->angularDragFactor = node.get("angularDragFactor", 1.f).asFloat();
    physics->gravityFactor = node.get("gravityFactor", 0.f).asFloat();
    physics->momentOfInertia = Json::LoadVec3(node["momentOfInertia"]);

    auto rigidBodyComp = physics->entity->GetComponent<Component::RigidBody>();
    auto shapeComp = physics->entity->GetComponent<Component::Shape>();
    if (rigidBodyComp && shapeComp) {
        rigidBodyComp->GetBulletRigidBody()->setCollisionShape(shapeComp->GetShape().GetShape());
    }

    return physics;
}

Component::RigidBody* PhysicsManager::CreateRigidBody(Entity* owner) {
    auto comp = rigidBodyComponents.Create();
    comp->entity = owner;

    comp->NewBulletRigidBody(1.0f);

    auto shapeComp = comp->entity->GetComponent<Component::Shape>();
    if (shapeComp) {
        comp->GetBulletRigidBody()->setCollisionShape(shapeComp->GetShape().GetShape());
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
        comp->GetBulletRigidBody()->setCollisionShape(shapeComp->GetShape().GetShape());
    }

    return comp;
}

Component::Shape* PhysicsManager::CreateShape(Entity* owner) {
    auto comp = shapeComponents.Create();
    comp->entity = owner;

    auto rigidBodyComp = comp->entity->GetComponent<Component::RigidBody>();
    if (rigidBodyComp) {
        rigidBodyComp->GetBulletRigidBody()->setCollisionShape(comp->GetShape().GetShape());
    }

    return comp;
}

Component::Shape* PhysicsManager::CreateShape(Entity* owner, const Json::Value& node) {
    auto comp = shapeComponents.Create();
    comp->entity = owner;

    if (node.isMember("sphere")) {
        auto sphere = node.get("sphere", {});
        auto radius = sphere.get("radius", 1.0f).asFloat();
        auto shape = new ::Physics::Shape(::Physics::Shape::Sphere(radius));
        comp->SetShape(shape);
    }
    else if (node.isMember("plane")) {
        auto plane = node.get("plane", {});
        auto normal = Json::LoadVec3(plane.get("normal", {}));
        auto planeCoeff = plane.get("planeCoeff", 0.0f).asFloat();
        auto shape = new ::Physics::Shape(::Physics::Shape::Plane(normal, planeCoeff));
        comp->SetShape(shape);
    }

    auto rigidBodyComp = comp->entity->GetComponent<Component::RigidBody>();
    if (rigidBodyComp) {
        rigidBodyComp->GetBulletRigidBody()->setCollisionShape(comp->GetShape().GetShape());
    }

    return comp;
}

void PhysicsManager::SetShape(Component::Shape* comp, ::Physics::Shape* shape) {
    comp->SetShape(shape);
}

void PhysicsManager::SetMass(Component::RigidBody* comp, float mass) {
    comp->Mass(mass);
}

const std::vector<Component::Physics*>& PhysicsManager::GetPhysicsComponents() const {
    return physicsComponents.GetAll();
}

const std::vector<Component::Shape*>& PhysicsManager::GetShapeComponents() const {
    return shapeComponents.GetAll();
}

void PhysicsManager::ClearKilledComponents() {
    physicsComponents.ClearKilled();
    rigidBodyComponents.ClearKilled(
        [this](Component::RigidBody* body) {
            dynamicsWorld->removeRigidBody(body->GetBulletRigidBody());
        });
    shapeComponents.ClearKilled();
}
