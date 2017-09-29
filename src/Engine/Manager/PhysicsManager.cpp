#include "PhysicsManager.hpp"

#include <btBulletDynamicsCommon.h>
#include <glm/gtx/quaternion.hpp>
#include "../Component/Physics.hpp"
#include "../Entity/Entity.hpp"
#include "../Physics/GlmConversion.hpp"
#include "../Physics/ITrigger.hpp"
#include "../Physics/RigidBody.hpp"
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

        physicsComp->GetRigidBody().Position(entity->position);
        dynamicsWorld->removeRigidBody(physicsComp->GetRigidBody().GetRigidBody());
        dynamicsWorld->addRigidBody(physicsComp->GetRigidBody().GetRigidBody());
        physicsComp->GetRigidBody().GetRigidBody()->setGravity(btVector3(0, 0, 0));
    }

    dynamicsWorld->stepSimulation(deltaTime, 10);

    for (auto trigger : triggers) {
        trigger->Process(*dynamicsWorld);
    }
}

void PhysicsManager::UpdateEntityTransforms() {
    std::vector<Component::Physics*> physicsObjects = physicsComponents.GetAll();
    for (Component::Physics* physicsComp : physicsObjects) {
        if (physicsComp->IsKilled() || !physicsComp->entity->enabled)
            continue;

        Entity* entity = physicsComp->entity;

        auto trans = physicsComp->GetRigidBody().GetRigidBody()->getWorldTransform();
        entity->position = Physics::btToGlm(trans.getOrigin());
        entity->rotation = glm::eulerAngles(Physics::btToGlm(trans.getRotation()));
    }
}

void PhysicsManager::OnTriggerEnter(Component::Physics* triggerBody, Component::Physics* object, std::function<void()> callback) {
    auto trigger = MakeTrigger(triggerBody);
    trigger->OnEnter(object, callback);
}

Physics::RigidBody* PhysicsManager::MakeRigidBody(Physics::Shape* shape, float mass) {
    auto body = new Physics::RigidBody(shape, mass);
    dynamicsWorld->addRigidBody(body->GetRigidBody());
    return body;
}

Physics::Trigger* PhysicsManager::MakeTrigger(Component::Physics* comp) {
    Physics::Trigger* trigger = new Physics::Trigger(comp);
    triggers.push_back(trigger);
    return trigger;
}

Component::Physics* PhysicsManager::CreatePhysics() {
    return physicsComponents.Create();
}

Component::Physics* PhysicsManager::CreatePhysics(const Json::Value& node) {
    Component::Physics* physics = physicsComponents.Create();
    
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
    
    auto shape = node.get("shape", {});
    if (shape.isMember("sphere")) {
        auto sphere = shape.get("sphere", {});
        auto radius = sphere.get("radius", 1.0f).asFloat();
        auto shape = new ::Physics::Shape(::Physics::Shape::Sphere(radius));
        physics->rigidBody = new ::Physics::RigidBody(shape, 1.0f);
    } else if (shape.isMember("plane")) {
        auto plane = shape.get("plane", {});
        auto normal = Json::LoadVec3(plane.get("normal", {}));
        auto planeCoeff = plane.get("planeCoeff", 0.0f).asFloat();
        auto shape = new ::Physics::Shape(::Physics::Shape::Plane(normal, planeCoeff));
        physics->rigidBody = new ::Physics::RigidBody(shape, 1.0f);
    }
    
    assert(physics->rigidBody);
    
    return physics;
}

const std::vector<Component::Physics*>& PhysicsManager::GetPhysicsComponents() const {
    return physicsComponents.GetAll();
}

void PhysicsManager::ClearKilledComponents() {
    physicsComponents.ClearKilled();
}
