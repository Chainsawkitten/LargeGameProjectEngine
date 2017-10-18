#pragma once

#include "SuperComponent.hpp"
#include <string>
#include <glm/glm.hpp>

namespace Animation {
    class AnimationController;
}

namespace Component {
    /// Animation controller.
    class AnimationController : public SuperComponent {
        public:
            /// Create new animation controller component.
            AnimationController();

            /// Save the component.
            /**
            * @return JSON value to be stored on disk.
            */
            Json::Value Save() const override;

            /// Update the animation controller.
            /**
             * @param deltaTime Time between frames.
             */
            void UpdateAnimation(float deltaTime);

            /// Returns a vector with the final calculated bones.
            /**
             * @return Vector with bones.
             */
            std::vector<glm::mat4> GetBones();

            Animation::AnimationController* controller = nullptr;
        private:

    };
}