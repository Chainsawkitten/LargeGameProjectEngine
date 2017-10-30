#pragma once

#include "SuperComponent.hpp"
#include <string>
#include <glm/glm.hpp>
#include "../Animation/AnimationController.hpp"

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
            ENGINE_API void UpdateAnimation(float deltaTime);

            /// Set a bool in the state machine.
            /**
             * @name Name of the bool.
             * @value Value to set.
             */
            ENGINE_API void SetBool(std::string name, bool value);

            /// Set a float in the state machine.
            /**
             * @name Name of the float.
             * @value Value to set.
             */
            ENGINE_API void SetFloat(std::string name, float value);

            /// Get a bool in the state machine.
            /**
             * @name Name of the bool.
             * @return Value of the bool.
             */
            ENGINE_API bool GetBool(std::string name);

            /// Get a float in the state machine.
            /**
             * @name Name of the float.
             * @return Value of the float.
             */
            ENGINE_API float GetFloat(std::string name);

            /// Vector with the final calculated bones.
            std::vector<glm::mat4> bones;

            /// The controller.
            Animation::AnimationController* controller = nullptr;

            /// The skeleton.
            Animation::Skeleton* skeleton = nullptr;

        private:
            Animation::AnimationController::AnimationAction* activeAction1 = nullptr;
            Animation::AnimationController::AnimationAction* activeAction2 = nullptr;
            Animation::AnimationController::AnimationTransition* activeTransition = nullptr;
    };
}