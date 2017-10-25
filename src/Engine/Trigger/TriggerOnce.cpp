#include "TriggerOnce.hpp"

TriggerOnce::TriggerOnce() {

}

TriggerOnce::~TriggerOnce() {

}

void TriggerOnce::OnEnter() {

}

void TriggerOnce::OnLeave() {

}

void TriggerOnce::OnRemain() {

}

std::string TriggerOnce::GetName() {
    return name;
}

void TriggerOnce::SetName(std::string value) {
    name = value;
}

std::string TriggerOnce::GetTargetFunction() {
    return targetFunction;
}

void TriggerOnce::SetTargetFunction(std::string value) {
    targetFunction = value;
}

bool TriggerOnce::GetStartActive() {
    return startActive;
}

void TriggerOnce::SetStartActive(bool value) {
    startActive = value;
}

float TriggerOnce::GetDelay() {
    return delay;
}

void TriggerOnce::SetDelay(float value) {
    delay = value;
}

Entity* TriggerOnce::GetTargetEntity() {
    return targetEntity;
}

void TriggerOnce::SetTargetEntity(Entity* value) {
    targetEntity = value;
}

Entity* TriggerOnce::GetCollidedEntity() {
    return collidedEntity;
}

void TriggerOnce::SetCollidedEntity(Entity* value) {
    collidedEntity = value;
}
