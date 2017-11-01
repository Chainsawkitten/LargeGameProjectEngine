#include "TriggerRepeat.hpp"

#include "../Component/RigidBody.hpp"
#include "../Entity/Entity.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/PhysicsManager.hpp"

TriggerRepeat::TriggerRepeat() {

}

TriggerRepeat::~TriggerRepeat() {

}

void TriggerRepeat::OnEnter() {
    if (collidedEntity && collidedEntity->GetComponent<Component::RigidBody>()) {
        Component::RigidBody* rigidBodyComp = collidedEntity->GetComponent<Component::RigidBody>();
        Managers().physicsManager->ForgetTriggerEnter(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerRetain(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerLeave(triggerVolume, rigidBodyComp);
        Managers().physicsManager->OnTriggerEnter(triggerVolume, rigidBodyComp, std::bind(&TriggerRepeat::HandleTriggerEvent, this));
    }
}

void TriggerRepeat::OnRetain() {
    if (collidedEntity && collidedEntity->GetComponent<Component::RigidBody>()) {
        Component::RigidBody* rigidBodyComp = collidedEntity->GetComponent<Component::RigidBody>();
        Managers().physicsManager->ForgetTriggerEnter(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerRetain(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerLeave(triggerVolume, rigidBodyComp);
        Managers().physicsManager->OnTriggerRetain(triggerVolume, rigidBodyComp, std::bind(&TriggerRepeat::HandleTriggerEvent, this));
    }
}

void TriggerRepeat::OnLeave() {
    if (collidedEntity && collidedEntity->GetComponent<Component::RigidBody>()) {
        Component::RigidBody* rigidBodyComp = collidedEntity->GetComponent<Component::RigidBody>();
        Managers().physicsManager->ForgetTriggerEnter(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerRetain(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerLeave(triggerVolume, rigidBodyComp);
        Managers().physicsManager->OnTriggerLeave(triggerVolume, rigidBodyComp, std::bind(&TriggerRepeat::HandleTriggerEvent, this));
    }
}

std::string TriggerRepeat::GetName() {
    return name;
}

void TriggerRepeat::SetName(std::string value) {
    name = value;
}

std::vector<std::string> TriggerRepeat::GetTargetFunction() {
    return targetFunction;
}

void TriggerRepeat::SetTargetFunction(std::string value) {
    targetFunction.push_back(value);
}

bool TriggerRepeat::GetStartActive() {
    return startActive;
}

void TriggerRepeat::SetStartActive(bool value) {
    startActive = value;
}

float TriggerRepeat::GetDelay() {
    return delay;
}

void TriggerRepeat::SetDelay(float value) {
    delay = value;
}

float TriggerRepeat::GetCooldown() {
    return cooldown;
}

void TriggerRepeat::SetCooldown(float value) {
    cooldown = value;
}

float TriggerRepeat::GetTriggerCharges() {
    return triggerCharges;
}

void TriggerRepeat::SetTriggerCharges(int value) {
    triggerCharges = value;
}

std::vector<Entity*> *TriggerRepeat::GetTargetEntity() {
    return &targetEntity;
}

Entity* TriggerRepeat::GetCollidedEntity() {
    return collidedEntity;
}

void TriggerRepeat::SetCollidedEntity(Entity* value) {
    collidedEntity = value;
}

void TriggerRepeat::HandleTriggerEvent() {
    triggered = true;
}

std::vector<EventStruct> *TriggerRepeat::GetEventVector() {
    return &eventVector;
}
