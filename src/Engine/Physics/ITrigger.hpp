#pragma once

#include <functional>

namespace Component {
    class RigidBody;
}

namespace Physics {

    /// Interface for triggers that can set up an observer for enter event.
    class ITrigger {
        public:
            /// Add listener for when a rigid body has entered a trigger volume.
            /**
             * @param body The body that is to enter the volume.
             * @param observer Event handler.
             */
            virtual void OnEnter(Component::RigidBody* body, std::function<void()> observer) = 0;
    };

}
