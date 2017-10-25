#include "TriggerRepeat.hpp"

TriggerRepeat::TriggerRepeat() {

}

TriggerRepeat::~TriggerRepeat() {

}

void TriggerRepeat::OnEnter() {
    // TODO
}

void TriggerRepeat::OnLeave() {
    // TODO
}


void TriggerRepeat::OnRemain() {
    // TODO
}

std::string TriggerRepeat::GetName() {
    return name;
}

void TriggerRepeat::SetName(std::string value) {
    name = value;
}

std::string TriggerRepeat::GetTargetFunction() {
    return targetFunction;
}

void TriggerRepeat::SetTargetFunction(std::string value) {
    targetFunction = value;
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

void TriggerRepeat::SetTriggerCharges(float value) {
    triggerCharges = value;
}

Entity* TriggerRepeat::GetTargetEntity() {
    return targetEntity;
}

void TriggerRepeat::SetTargetEntity(Entity* value) {
    targetEntity = value;
}

Entity* TriggerRepeat::GetCollidedEntity() {
    return collidedEntity;
}

void TriggerRepeat::SetCollidedEntity(Entity* value) {
    collidedEntity = value;
}
