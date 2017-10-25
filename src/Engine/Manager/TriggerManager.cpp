#include "TriggerManager.hpp"
#include "../Component/Trigger.hpp"
#include "../Trigger/SuperTrigger.hpp"

TriggerManager::TriggerManager() {

}

TriggerManager::~TriggerManager() {

}

Component::Trigger* TriggerManager::CreateTrigger() {
    return triggerComponents.Create();
}

void TriggerManager::ClearKilledComponents() {
    triggerComponents.ClearKilled();
}

const std::vector<Component::Trigger*>& TriggerManager::GetTriggerComponents() {
    return triggerComponents.GetAll();
}

void TriggerManager::CreateTrigger(SuperTrigger* trigger) {
    triggerComponents.Create();
}
