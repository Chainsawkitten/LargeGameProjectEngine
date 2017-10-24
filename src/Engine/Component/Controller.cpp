#include "Controller.hpp"
#include "Manager/Managers.hpp"

namespace Component {
    Controller::Controller() {
        controllerID = 1;
    }

    Controller::~Controller() {
    }

    Json::Value Controller::Save() const {
        Json::Value component;

        component["controllerID"] = controllerID;

        return component;
    }

    glm::mat4 Controller::HandleTransformation(Entity* entity) {
        glm::mat4 ctrlTransform = Managers().vrManager->GetControllerPoseMatrix(controllerID);
        glm::vec3 ctrlRight = glm::vec3(ctrlTransform[0][0], ctrlTransform[1][0], ctrlTransform[2][0]);
        glm::vec3 ctrlUp = glm::vec3(ctrlTransform[0][1], ctrlTransform[1][1], ctrlTransform[2][1]);
        glm::vec3 ctrlForward = glm::vec3(ctrlTransform[0][2], ctrlTransform[1][2], ctrlTransform[2][2]);
        glm::vec3 ctrlPosition = glm::vec3(-ctrlTransform[3][0], -ctrlTransform[3][1], -ctrlTransform[3][2]);

        glm::mat4 ctrlOrientation = glm::mat4(
            glm::vec4(ctrlRight, 0.f),
            glm::vec4(ctrlUp, 0.f),
            glm::vec4(ctrlForward, 0.f),
            glm::vec4(0.f, 0.f, 0.f, 1.f));

        glm::vec3 localPosition = ctrlPosition * Managers().vrManager->GetScale();
        glm::mat4 localTranslationMatrix = glm::translate(glm::mat4(), localPosition);
        glm::mat4 globalTranslationMatix = entity->GetModelMatrix() * (ctrlOrientation * localTranslationMatrix);

        return globalTranslationMatix;
    }

    bool Controller::HandleInput(int buttonID) {
        vr::EVRButtonId vrButton = static_cast<vr::EVRButtonId>(buttonID);

        return Managers().vrManager->GetInput(vrButton);
    }
}
