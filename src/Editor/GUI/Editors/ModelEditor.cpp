#include "ModelEditor.hpp"

#include <Engine/Geometry/Model.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;

void ModelEditor::Show() {
    if (ImGui::Begin(("Model: " + model->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(model))).c_str(), &visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders)) {
        ImGui::InputText("Name", name, 128);
        model->name = name;

        if (ImGui::Button("Open model")) {
            fileSelector.AddExtensions("fbx");
            fileSelector.AddExtensions("md5mesh");
            fileSelector.SetFileSelectedCallback(std::bind(&ModelEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }

        if (hasSourceFile) {
            ImGui::Checkbox("Triangulate", &triangulate);
            ImGui::Checkbox("Import Normals", &importNormals);
            ImGui::Checkbox("Import Tangents", &importTangents);

            if (ImGui::Button("Import")) {
                AssetConverter asset;
                asset.Convert(source.c_str(), destination.c_str(), triangulate, importNormals, importTangents);
                model->Load(destination.c_str());
            }
        }
    }
    ImGui::End();

    if (fileSelector.IsVisible())
        fileSelector.Show();
}

const Geometry::Model* ModelEditor::GetModel() const {
    return model;
}

void ModelEditor::SetModel(Geometry::Model* model) {
    this->model = model;

    strcpy(name, model->name.c_str());
}

bool ModelEditor::IsVisible() const {
    return visible;
}

void ModelEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void ModelEditor::FileSelected(const std::string& file) {
    destination = Hymn().GetPath() + FileSystem::DELIMITER + "Models" + FileSystem::DELIMITER + model->name + ".asset";
    source = file;
    hasSourceFile = true;
}
