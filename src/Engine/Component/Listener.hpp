#pragma once

#include "SuperComponent.hpp"
#include "../linking.hpp"

namespace Component {
    /// %Component describing a listener that can listen to sounds.
    class Listener : public SuperComponent {
        public:
        /// Create new listener.
        ENGINE_API Listener();

        /// Save the component.
        /**
             * @return JSON value to be stored on disk.
             */
        ENGINE_API Json::Value Save() const override;
    };
}
