#include "AnimationController.hpp"

namespace Component {
    AnimationController::AnimationController() {
    }

    Json::Value AnimationController::Save() const {
        return Json::Value();
    }

    std::vector<glm::mat4> Component::AnimationController::GetBones() {
        return std::vector<glm::mat4>();
    }

    void Component::AnimationController::UpdateAnimation(float deltaTime) {
    }
}
