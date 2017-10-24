#pragma once
#include "SuperComponent.hpp"
#include "../Manager/VRManager.hpp"
#include "../Entity/Entity.hpp"
#include <glm/glm.hpp>
#include <openvr.h>
#include <glm/gtc/matrix_transform.hpp>
#include "../linking.hpp"

namespace Component {
    /// Component giving VR controller functions to an Entity
    class Controller : public SuperComponent {
        public:
        /// Constructor
        ENGINE_API Controller();

        /// Destructor
        ENGINE_API ~Controller();

        /// Save the component.
        /**
             * @return JSON value to be stored on disk.
             */
        ENGINE_API Json::Value Save() const override;

        /// Get's the VR controller's transformation matrix
        /**
             * @param The entity who's controller it is we're handling.
             * @return The transformation matrix.
             */
        ENGINE_API glm::mat4 HandleTransformation(Entity* entity);

        /// Handles all VR controller inputs (class for future implementations)
        /**
             * @param buttonID The id of the button to be handled.
             * @return Whether the checked button was pressed or not.
             */
        ENGINE_API bool HandleInput(int buttonID);

        /// The controller's ID. 1 = left, 2 = right
        int controllerID;

        private:
        glm::mat4 transform;
    };
}
