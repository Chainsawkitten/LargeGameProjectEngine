#include "SavePromptWindow.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;
using namespace std;

void SavePromptWindow::Show() {
    ImGui::OpenPopup("Save before you quit?");

    if (ImGui::BeginPopupModal("Save before you quit?", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_ShowBorders)) {
    
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            decision = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Don't save", ImVec2(120, 0))) {
            decision = 1;
            ImGui::CloseCurrentPopup();
        }
        //if (ImGui::Button("cancel", ImVec2(120, 0))) {
        //    decision = 2;
        //    ImGui::CloseCurrentPopup();
        //}
        ImGui::EndPopup();
        
    }

}

int SavePromptWindow::GetDecision() const {

    return decision;

}

bool SavePromptWindow::IsVisible() const {
    return visible;
}

void SavePromptWindow::SetVisible(bool visible) {
    this->visible = visible;
}