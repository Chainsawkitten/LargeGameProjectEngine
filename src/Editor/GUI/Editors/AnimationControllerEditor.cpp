#include "AnimationControllerEditor.hpp"
#include <Engine/Animation/AnimationController.hpp>
#include <Utility/Log.hpp>

AnimationControllerEditor::AnimationControllerEditor() {
}

AnimationControllerEditor::~AnimationControllerEditor() {
}

void AnimationControllerEditor::SetAnimationController(Animation::AnimationController * animationController) {
    this->animationController = animationController;
}

Animation::AnimationController * AnimationControllerEditor::Get() {
    return animationController;
}

void AnimationControllerEditor::ShowContextMenu() {
    if (animationController == nullptr) {
        Log() << "ERROR: No animation controller choosen.\n";
        return;
    }

    if (ImGui::MenuItem("Add animation action")) {
        Animation::AnimationController::AnimationAction* newAction = new Animation::AnimationController::AnimationAction;
        std::string name = "Action #" + std::to_string(animationController->animationNodes.size() + 1);
        memcpy(newAction->name, name.c_str(), name.size() + 1);
        newAction->index = animationController->animationNodes.size();
        animationController->animationNodes.push_back(newAction);
    }

    if (ImGui::MenuItem("Add animation transition")) {
        Animation::AnimationController::AnimationTransition* newTransition = new Animation::AnimationController::AnimationTransition;
        std::string name = "Animation transition: " + std::to_string(animationController->animationNodes.size() + 1);
        memcpy(newTransition->name, name.c_str(), name.size() + 1);
        animationController->animationNodes.push_back(newTransition);
    }
}

void AnimationControllerEditor::ShowNode(Node * node) {
    ImGui::Text("Action: %s", node->name);
    ImGui::InputText("Name", node->name, 128);
}

Node** AnimationControllerEditor::GetNodeArray() {
    return &animationController->animationNodes[0];
}

unsigned int AnimationControllerEditor::GetNumNodes() {
    if (animationController == nullptr) {
        Log() << "ERROR: No animation controller choosen.\n";
        return 0;
    }

    return animationController->animationNodes.size();
}

bool AnimationControllerEditor::CanConnect(Node * output, Node * input) {
    if (typeid(*output) == typeid(Animation::AnimationController::AnimationAction)) {
        if (typeid(*input) == typeid(Animation::AnimationController::AnimationTransition))
            return true;

        return false;
    }
    else if (typeid(*output) == typeid(Animation::AnimationController::AnimationTransition)) {
        if (typeid(*input) == typeid(Animation::AnimationController::AnimationAction))
            return true;

        return false;
    }
    
    return false;
}
