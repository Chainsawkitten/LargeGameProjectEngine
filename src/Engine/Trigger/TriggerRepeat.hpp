#pragma once

#include "SuperTrigger.hpp"

class TriggerRepeat : public SuperTrigger {
    public:
        ENGINE_API TriggerRepeat();
        ENGINE_API ~TriggerRepeat();

        ENGINE_API void OnEnter();
        ENGINE_API void OnLeave();
        ENGINE_API void OnRemain();

        ENGINE_API std::string GetName();
        ENGINE_API void SetName(std::string value);

        ENGINE_API std::string GetTargetFunction();
        ENGINE_API void SetTargetFunction(std::string value);

        ENGINE_API bool GetStartActive();
        ENGINE_API void SetStartActive(bool value);

        ENGINE_API float GetDelay();
        ENGINE_API void SetDelay(float value);

        ENGINE_API float GetCooldown();
        ENGINE_API void SetCooldown(float value);

        ENGINE_API float GetTriggerCharges();
        ENGINE_API void SetTriggerCharges(float value);

        ENGINE_API Entity* GetTargetEntity();
        ENGINE_API void SetTargetEntity(Entity* value);

        ENGINE_API Entity* GetCollidedEntity();
        ENGINE_API void SetCollidedEntity(Entity* value);

    private:
        std::string name = "DEBUG";
        std::string targetFunction = "DEBUG";
        bool startActive = false;
        float delay = 0;
        float cooldown = 0;
        float triggerCharges = 0;
        Entity* targetEntity = nullptr;
        Entity* collidedEntity = nullptr;
};
