#include "SceneEditor.hpp"

#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Utility/Log.hpp>
#include <imgui.h>
#include "../../Resources.hpp"

using namespace GUI;

enum DraggedItemState {
    NOT_ACTIVE = 0,
    ACTIVATED_THIS_FRAME,
    ACTIVE,
    DEACTIVATE,
    DEACTIVATED_THIS_FRAME
};
DraggedItemState draggedItemState = DraggedItemState::NOT_ACTIVE;

SceneEditor::SceneEditor() {
    name[0] = '\0';
}

void SceneEditor::Show() {
    if (ImGui::Begin(("Scene: " + *scene + "###Scene").c_str(), &visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders)) {
        ImGui::InputText("Name", name, 128);
        *scene = name;
        
        // Entities.
        entityPressed = false;
        ShowEntity(Hymn().world.GetRoot());

        switch (draggedItemState) {

        case NOT_ACTIVE:
            if (draggedEntity != nullptr)
                draggedItemState = DraggedItemState::ACTIVATED_THIS_FRAME;
            break;

        case ACTIVATED_THIS_FRAME:
            draggedItemState = DraggedItemState::ACTIVE;
            break;

        case ACTIVE:
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(450.0f);
            ImGui::TextUnformatted(draggedEntity->name.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
            break;

        case DEACTIVATE:
            draggedItemState = DEACTIVATED_THIS_FRAME;
            break;

        case DEACTIVATED_THIS_FRAME:
            draggedItemState = DraggedItemState::NOT_ACTIVE;
            draggedEntity = nullptr;
            break;

        default:
            break;

        }

    }
    ImGui::End();
}

void SceneEditor::SetScene(std::string* scene) {
    entityEditor.SetVisible(false);
    this->scene = scene;
    
    if (scene != nullptr) {
        strcpy(name, scene->c_str());
    } else {
        SetVisible(false);
    }
}

bool SceneEditor::IsVisible() const {
    return visible;
}

void SceneEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void SceneEditor::Save() const {
    if (scene != nullptr)
        Hymn().world.Save(Hymn().GetPath() + FileSystem::DELIMITER + "Scenes" + FileSystem::DELIMITER + *scene + ".json");
}

void SceneEditor::ShowEntity(Entity* entity) {
    bool leaf = entity->IsScene() || entity->GetChildren().empty();
    bool opened = ImGui::TreeNodeEx(entity->name.c_str(), leaf ? ImGuiTreeNodeFlags_Leaf : 0);
    bool instantiate = false;

    //If we pressed down on and started dragging on the entity.
    if (draggedEntity == nullptr && ImGui::IsItemActive())
        draggedEntity = entity;

    if (draggedItemState == DraggedItemState::ACTIVE && draggedEntity == entity && !ImGui::IsItemActive())
        draggedItemState = DraggedItemState::DEACTIVATE;
    
    if (draggedItemState == DraggedItemState::DEACTIVATED_THIS_FRAME && ImGui::IsItemHovered() && draggedEntity != entity)
        draggedEntity->SetParent(entity);

    if (ImGui::BeginPopupContextItem(entity->name.c_str())) {
        if (ImGui::Selectable("Edit")) {
            entityPressed = true;
            entityEditor.SetEntity(entity);
        }
        
        if (!entity->IsScene()) {
            if (ImGui::Selectable("Add child")) 
                entity->AddChild("Entity #" + std::to_string(Hymn().entityNumber++));
            
            if (ImGui::Selectable("Instantiate scene"))
                instantiate = true;
        }
        
        if (entity != Hymn().world.GetRoot()) {
            if (ImGui::Selectable("Delete")) {
                entity->Kill();
                if (entityEditor.ShowsEntity(entity))
                    entityEditor.SetVisible(false);
            }
        }
        
        ImGui::EndPopup();
    }
    
    if (instantiate)
        ImGui::OpenPopup("Select scene");
    
    if (ImGui::BeginPopup("Select scene")) {
        ImGui::Text("Scenes");
        ImGui::Separator();
        
        if (sceneSelector.Show(ResourceList::Resource::SCENE)) {
            std::string name = sceneSelector.GetSelectedResource().resource->scene;
            entity->InstantiateScene(sceneSelector.GetSelectedResource().path + "/" + name, Resources().activeScene);
        }
        
        ImGui::EndPopup();
    }
    
    if (opened) {
        if (!entity->IsScene()) {
            for (Entity* child : entity->GetChildren()) {
                ShowEntity(child);
            }
        }
        
        ImGui::TreePop();
    }
}
