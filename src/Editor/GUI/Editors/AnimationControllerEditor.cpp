#include "AnimationControllerEditor.hpp"
#include <Engine/Animation/AnimationController.hpp>
#include <Utility/Log.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Hymn.hpp>

using namespace GUI;

AnimationControllerEditor::AnimationControllerEditor() {
}

AnimationControllerEditor::~AnimationControllerEditor() {
}

void AnimationControllerEditor::SetAnimationController(Animation::AnimationController * animationController) {
    if (this->animationController != nullptr)
        this->animationController->Save(Hymn().GetPath() + "/" + animationController->path + animationController->name + ".asset");

    this->animationController = animationController;
}

Animation::AnimationController * AnimationControllerEditor::GetAnimationController() {
    return animationController;
}

void AnimationControllerEditor::SetVisible(bool visible) {
    NodeEditor::SetVisible(visible);
    if (this->animationController != nullptr)
        animationController->Save(Hymn().GetPath() + "/" + animationController->path + animationController->name + ".asset");
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

    if (typeid(*node) == typeid(Animation::AnimationController::AnimationAction)) {
        Animation::AnimationController::AnimationAction * action = dynamic_cast<Animation::AnimationController::AnimationAction*>(node);
        if (ImGui::Button("Select animation clip##Clip"))
            ImGui::OpenPopup("Select animation clip##Clip");

        if (ImGui::BeginPopup("Select animation clip##Clip")) {
            ImGui::Text("Select animation");
            ImGui::Separator();

            if (resourceSelector.Show(ResourceList::Resource::Type::ANIMATION_CLIP)) {
                if (action->animationClipName != nullptr)
                    Managers().resourceManager->FreeAnimationClip(action->animationClip);
                    
                std::string path = resourceSelector.GetSelectedResource().GetPath();
                action->animationClip = Managers().resourceManager->CreateAnimationClip(path);
                memcpy(action->animationClipName, path.c_str(), path.size() + 1);
            }

            ImGui::EndPopup();
        }
    }
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
    } else if (typeid(*output) == typeid(Animation::AnimationController::AnimationTransition)) {
        if (typeid(*input) == typeid(Animation::AnimationController::AnimationAction))
            return true;

        return false;
    }

    return false;
}
