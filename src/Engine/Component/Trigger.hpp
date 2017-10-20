#pragma once

#include "SuperComponent.hpp"

namespace Component {
    class Trigger : public SuperComponent {
        public:
            enum TriggerTypes {
                ONCE = 0,
                REPEAT,
                LOOK_AT,
                PROXIMITY,
                NUMBER_OF_TYPES
            };

            Trigger();

            ~Trigger() override;

            Json::Value Save() const override;

            TriggerTypes triggerType = TriggerTypes::ONCE;
    };
}
