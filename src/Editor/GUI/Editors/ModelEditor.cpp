#include "ModelEditor.hpp"

#include <Engine/Geometry/Model.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>
#include "Util/AssetMetaData.hpp"
#include <Utility/Log.hpp>
#include "../../Resources.hpp"
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>

using namespace GUI;

ModelEditor::ModelEditor() {
    name[0] = '\0';
}

void ModelEditor::Show() {
    if (ImGui::Begin(("Model: " + model->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(model))).c_str(), &visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders)) {
        if (ImGui::InputText("Name", name, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
            model->name = name;

            // Rename all the files.
            if (FileSystem::FileExists((destination + ".fbx").c_str())) {
                FileSystem::Rename(destination + ".fbx", std::string(name) + ".fbx");

                if (FileSystem::FileExists((destination + ".asset").c_str()))
                    FileSystem::Rename(destination + ".asset", std::string(name) + ".asset");

                if (FileSystem::FileExists((destination + ".asset.meta").c_str()))
                    FileSystem::Rename(destination + ".asset.meta", std::string(name) + ".asset.meta");

                destination = Hymn().GetPath() + FileSystem::DELIMITER + "Models" + FileSystem::DELIMITER + name;
            }
        }

        if (ImGui::Button("Open model")) {
            // Currently only fbx is tested.
            fileSelector.AddExtensions("fbx");

            // Set the initial path to the models directory.
            fileSelector.SetInitialPath((Hymn().GetPath() + FileSystem::DELIMITER + "Models").c_str());
            fileSelector.SetFileSelectedCallback(std::bind(&ModelEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }

        if (hasSourceFile) {
            ImGui::Text("Mesh Data");
            ImGui::Checkbox("Uniform Scaling", &uniformScaling);

            if (uniformScaling) {
                float uniScale = scale.x;
                ImGui::DragFloat("Scale", &uniScale, 0.01f);
                scale = glm::vec3(uniScale);
            } else
                ImGui::DragFloat3("Scale", &scale[0], 0.01f);
            
            ImGui::Checkbox("Triangulate", &triangulate);
            ImGui::Checkbox("Import Normals", &importNormals);
            ImGui::Checkbox("Import Tangents", &importTangents);
            ImGui::Checkbox("Import Textures", &importTextures);
            ImGui::Checkbox("Flip UVs", &flipUVs);

            std::string button = isImported ? "Re-import" : "Import";

            if (ImGui::Button(button.c_str())) {
                AssetConverter::Materials materials;
                
                // Convert to .asset format.
                AssetConverter asset;
                asset.Convert(source.c_str(), (destination + ".asset").c_str(), scale, triangulate, importNormals, importTangents, flipUVs, importTextures, materials);
                model->Load(destination.c_str());
                msgString = asset.Success() ? "Success\n" : asset.GetErrorString();
                isImported = true;

                // Generate meta data.
                AssetMetaData::MeshImportData* importData = new AssetMetaData::MeshImportData;
                importData->triangulate = triangulate;
                importData->importNormals = importNormals;
                importData->importTangents = importTangents;
                AssetMetaData::GenerateMetaData((destination + ".asset.meta").c_str(), importData);
                
                // Import textures.
                if (importTextures) {
                    LoadTexture(materials.albedo, "Albedo");
                    LoadTexture(materials.normal, "Normal");
                    LoadTexture(materials.roughness, "Roughness");
                    LoadTexture(materials.metallic, "Metallic");
                }

                delete importData;
            }

            if (isImported)
                ImGui::Text(msgString.c_str());
        }
    }
    ImGui::End();

    if (fileSelector.IsVisible())
        fileSelector.Show();
}

const Geometry::Model* ModelEditor::GetModel() const {
    return model;
}

void ModelEditor::SetModel(ResourceList::ResourceFolder* folder, Geometry::Model* model) {
    this->folder = folder;
    this->model = model;

    strcpy(name, model->name.c_str());

    destination = Hymn().GetPath() + "/" + model->path + name;

    RefreshImportSettings();
}

bool ModelEditor::IsVisible() const {
    return visible;
}

void ModelEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void ModelEditor::FileSelected(const std::string& file) {
    std::string name = FileSystem::GetName(file).c_str();

    // Checking so that the file isn't imported twice.
    /// @todo Overwrite option?
    /// @todo Reimplement this.
    /*for (int i = 0; i < Resources().models.size(); ++i) {
        if (Resources().models[i]->name == name) {
            Log() << "File " << name << " is already added to project.\n";
            isImported = false;
            hasSourceFile = false;
            return;
        }
    }*/

    source = file;

    // Rename the model to the name of the source file.
    strcpy(this->name, name.c_str());
    model->name = this->name;

    destination = Hymn().GetPath() + "/" + model->path + name;

    // Check if source file is in proper directory, otherwise, copy it.
    if (!FileSystem::FileExists((destination + "." + FileSystem::GetExtension(source)).c_str()))
        FileSystem::Copy(source.c_str(), (destination + "." + FileSystem::GetExtension(source)).c_str());

    hasSourceFile = true;

    RefreshImportSettings();
}

void ModelEditor::RefreshImportSettings() {
    // Check if the source file exist, currently only .fbx is supported.
    if (!FileSystem::FileExists((destination + ".fbx").c_str())) {
        hasSourceFile = false;
        isImported = false;
        return;
    }

    // Check if meta file already exists. If it does import the metadata.
    std::string filePath(destination + ".asset.meta");
    if (FileSystem::FileExists(filePath.c_str())) {
        AssetMetaData::MeshImportData * importData = AssetMetaData::GetMetaData(filePath.c_str());
        triangulate = importData->triangulate;
        importNormals = importData->importNormals;
        importTangents = importData->importTangents;

        delete importData;
        isImported = true;
    } else {
        isImported = false;
    }
}

void ModelEditor::LoadTexture(const std::string& path, const std::string& name) {
    if (!path.empty()) {
        std::string textureName = model->name + name;
        std::string src = FileSystem::GetDirectory(source) + path;
        std::string dest = model->path + textureName;
        
        // Copy file.
        FileSystem::Copy(src.c_str(), (Hymn().GetPath() + "/" + dest + ".png").c_str());
        
        // Add texture asset.
        ResourceList::Resource resource;
        resource.type = ResourceList::Resource::TEXTURE;
        resource.texture = Managers().resourceManager->CreateTextureAsset(dest);
        folder->resources.push_back(resource);
    }
}
