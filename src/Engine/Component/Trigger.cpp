#include "Trigger.hpp"
#include "../Trigger/SuperTrigger.hpp"

using namespace Component;

Trigger::Trigger() {
    // ADD CONSTRUCTOR
}

Trigger::~Trigger() {
    // ADD DECONSTRUCT
}

Json::Value Trigger::Save() const {
    // ADD SAVE
    Json::Value component;

    return component;
}

SuperTrigger* Trigger::GetTrigger() {
    return m_trigger;
}

void Trigger::SetTrigger(SuperTrigger* trigger) {
    delete m_trigger;
    m_trigger = trigger;
}
