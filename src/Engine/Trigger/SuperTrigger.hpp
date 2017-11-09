#pragma once

namespace Json {
    class Value;
}

class SuperTrigger {
    /// %Super class for triggers to inherit from.
    public:

        /// Create new %SuperTrigger.
        SuperTrigger();

        /// Destructor.
        virtual ~SuperTrigger();

        /// Process the trigger in case of collision.
        virtual void Process() = 0;

        /// Update position for trigger volume.
        virtual void Update() = 0;

        /// Save the trigger.
        /**
         * @return JSON value to be stored on disk.
         */
        virtual Json::Value Save() = 0;

        /// Initialize entity references for triggers via entity UIDs.
        virtual void InitTriggerUID() = 0;
};
