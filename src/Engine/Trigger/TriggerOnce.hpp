#pragma once

#include <string>
#include "Entity/Entity.hpp"

/// @todo: This pollutes global namespace. Should probably be placed inside TriggerOnce.
enum Output {
    OnEnter,
    OnLeave
};

class TriggerOnce {
    public:
        TriggerOnce();

        ~TriggerOnce();

        void Execute(Output type);

    private:
        void OnEnter();
        void OnLeave();

        std::string name;
        std::string targetFunction;
        bool startActive;
        float delay;
        Entity* targetEntity;
        Entity* collidedEntity;
};
