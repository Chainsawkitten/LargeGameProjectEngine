#include "TriggerEditor.hpp"

#include <array>
#include <Engine/Component/Trigger.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/TriggerManager.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Trigger/TriggerRepeat.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Entity/Entity.hpp>
#include <imgui.h>

// This is necessary to use std::string in ImGui::Combo()
namespace ImGui {

    static auto vector_getter = [](void* vec, int idx, const char** out_text) {
        auto& vector = *static_cast<std::vector<std::string>*>(vec);
        if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
        *out_text = vector.at(idx).c_str();
        return true;
    };

    bool Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
        if (values.empty()) { return false; }
        return Combo(label, currIndex, vector_getter,
            static_cast<void*>(&values), values.size());
    }
}

namespace GUI {
    void TriggerEditor::Open() {
        ImGui::SetNextWindowPosCenter();
        ImGui::OpenPopup("trigger-edit");
    }

    void TriggerEditor::Show(Component::Trigger& comp) {
        if (ImGui::BeginPopupModal("trigger-edit", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_ShowBorders)) {
            if (ImGui::RadioButton("Properties", selectedTab == 0))
                selectedTab = 0;

            ImGui::SameLine();

            if (ImGui::RadioButton("Subjects", selectedTab == 1))
                selectedTab = 1;

            ImGui::SameLine();

            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();

            ImGui::Separator();

            // Working under the assumption that the internal trigger
            // is set and is indeed a repeat trigger.
            auto repeat = Managers().triggerManager->GetTriggerRepeat(comp);
            assert(repeat);

            switch (selectedTab) {
            case 0: {
                std::array<char, 100> name;
                float delay = repeat->GetDelay();
                float cooldown = repeat->GetCooldown();
                int charges = repeat->GetTriggerCharges();
                int startActive = repeat->GetStartActive();


                memcpy(name.data(), repeat->GetName().c_str(), std::min(repeat->GetName().size(), name.size()));
                name[std::min(repeat->GetName().size(), name.size() - 1)] = '\0';


                // NAME
                if (ImGui::InputText("Name", name.data(), name.size()))
                    repeat->SetName(name.data());

                // DELAY
                if (ImGui::InputFloat("Delay", &delay, 0.1f, 1.0f, 1)) {

                    if (delay < 0.0f)
                        delay = 0.0f;

                    repeat->SetDelay(delay);
                }
                ImGui::SameLine();
                ImGui::Text("[?]");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Time in seconds to delay.\nHold button to increase speed.");

                // COOLDOWN
                if (ImGui::InputFloat("Cooldown", &cooldown, 0.1f, 1.0f, 1)) {

                    if (cooldown < 0.0f)
                        cooldown = 0.0f;

                    repeat->SetCooldown(cooldown);
                }

                ImGui::SameLine();
                ImGui::Text("[?]");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Time in seconds to cooldown.\nHold button to increase speed.");

                // CHARGES                
                if (ImGui::InputInt("Charges", &charges)) {

                    if (charges < 0)
                        charges = 0;

                    repeat->SetTriggerCharges(charges);
                }
                ImGui::SameLine();

                ImGui::Text("[?]");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("How many times a trigger can execute before automatically going inactive.\n0 = INFINITE.\nHold button to increase speed.");

                // START ACTIVE
                ImGui::Text("Start active? ");

                ImGui::SameLine();

                if (ImGui::RadioButton("Yes", &startActive, 1)) {
                    repeat->SetStartActive(true);
                }

                ImGui::SameLine();

                if (ImGui::RadioButton("No", &startActive, 0)) {
                    repeat->SetStartActive(false);
                }

                break;
            }
            case 1: {

                // Hardcoded single event for demonstration purposes
                if (ImGui::Button("New event", ImVec2(100, 25))) {
                    EventStruct newStruct;
                    repeat->GetEventVector()->push_back(newStruct);
                }

                ImGui::NewLine();
                ImGui::Columns(4);

                ImGui::Text("Event");
                ImGui::NextColumn();
                ImGui::Text("Subject");
                ImGui::NextColumn();
                ImGui::Text("Target entity");
                ImGui::NextColumn();
                ImGui::Text("Script method");
                ImGui::Separator();

                for (int i = 0; i < repeat->GetEventVector()->size(); i++) {

                    if (repeat->GetEventVector()->at(i).isGettingRemoved == true) {
                        repeat->GetEventVector()->erase(repeat->GetEventVector()->begin() + i);
                        repeat->GetTargetEntity()->erase(repeat->GetTargetEntity()->begin() + i);
                        break;
                    }
                }

                for (int i = 0; i < repeat->GetEventVector()->size(); i++) {
                    ImGui::Columns(4);

                    std::array<const char*, 3> events = {
                        "OnEnter",
                        "OnRetain",
                        "OnLeave",
                    };

                    std::string labelEvent = "Type ";
                    labelEvent.append(std::to_string(i));
                    std::string labelTarget = "Entity ";
                    labelTarget.append(std::to_string(i));

                    int eventType = repeat->GetEventVector()->at(i).m_eventID;

                    if (ImGui::Combo(labelEvent.c_str(), &eventType, events.data(), events.size())) {
                        repeat->GetEventVector()->at(i).m_eventID = eventType;
                    }


                    ImGui::NextColumn();

                    ImGui::Text("[Select subject rigid body here]");

                    ImGui::NextColumn();
                    int targetID = repeat->GetEventVector()->at(i).m_targetID;
                    std::vector<std::string> entityName;

                    for (int i = 0; i < Hymn().world.GetEntities().size(); i++) {

                        entityName.push_back(Hymn().world.GetEntities().at(i)->name);
                    }

                    if (ImGui::Combo(labelTarget.c_str(), &targetID, entityName)) {
                        repeat->GetEventVector()->at(i).m_targetID = targetID;

                        for (int i = 0; i < Hymn().world.GetEntities().size(); i++) {

                            if (Hymn().world.GetEntities().at(i)->name == entityName.at(targetID) && entityName.at(targetID) != comp.entity->name) {
                                repeat->GetTargetEntity()->push_back(Hymn().world.GetEntities().at(i));

                            }
                        }
                    }

                    if (entityName.at(targetID) == comp.entity->name) {
                        ImGui::SameLine();
                        ImGui::PushStyleColor(0, ImVec4(1.0, 0.0, 0.0, 1.0));
                        ImGui::Text("[?]");

                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Cannot select the Entity this trigger belongs to.");

                        ImGui::PopStyleColor();
                    }

                    ImGui::NextColumn();
                    ImGui::Text("[Select script method here]");

                    ImGui::SameLine();
                    if (ImGui::Button("Remove")) {
                        repeat->GetEventVector()->at(i).isGettingRemoved = true;
                    }

                    ImGui::Separator();
                }
                break;
            }
            }

            ImGui::EndPopup();
        }
    }
}
