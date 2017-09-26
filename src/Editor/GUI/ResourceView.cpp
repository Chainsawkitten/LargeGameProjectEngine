#include "ResourceView.hpp"

#include <Engine/Geometry/Model.hpp>
#include <Engine/Texture/TextureAsset.hpp>
#include <Engine/Audio/SoundBuffer.hpp>
#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Editor/Util/EditorSettings.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/MainWindow.hpp>
#include <imgui.h>
#include <limits>
#include "../ImGui/Splitter.hpp"
#include "../Resources.hpp"
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <cstdio>
#include <Utility/Log.hpp>

using namespace GUI;
using namespace std;

ResourceView::ResourceView() {

}

void ResourceView::Show() {
    ImVec2 size(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y);
    
    // Splitter.
    ImGui::VerticalSplitter(ImVec2(sceneWidth, size.y - resourceHeight), size.x - sceneWidth - editorWidth, splitterSize, resourceHeight, resourceResize, 20, size.y - 20);
    if (resourceResize)
        resourceHeight = size.y - resourceHeight;
    
    ImGui::SetNextWindowPos(ImVec2(sceneWidth, size.y - resourceHeight));
    ImGui::SetNextWindowSize(ImVec2(size.x - sceneWidth - editorWidth, resourceHeight));
    
    ImGui::Begin("Resources", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders);
    
    // Scenes.
    if (ImGui::TreeNode("Scenes")) {
        if (ImGui::Button("Add scene"))
            Resources().scenes.push_back("Scene #" + std::to_string(Resources().scenes.size()));
        
        for (std::size_t i = 0; i < Resources().scenes.size(); ++i) {
            if (ImGui::Selectable(Resources().scenes[i].c_str())) {
                changeScene = true;
                sceneIndex = i;
                savePromptWindow.SetTitle("Save before you switch scene?");
            }
            
            if (ImGui::BeginPopupContextItem(Resources().scenes[i].c_str())) {
                if (ImGui::Selectable("Delete")) {
                    Resources().scenes.erase(Resources().scenes.begin() + i);
                    ImGui::EndPopup();
                    
                    if (Resources().activeScene >= i) {
                        if (Resources().activeScene > 0)
                            Resources().activeScene = Resources().activeScene - 1;
                        
                        sceneEditor.SetScene(Resources().activeScene);
                    }
                    break;
                }
                ImGui::EndPopup();
            }
        }
        ImGui::TreePop();

        if (changeScene) {

            if (Hymn().GetPath() != "") {

                savePromptWindow.SetVisible(true);
                savePromptWindow.Show();

                switch (savePromptWindow.GetDecision())
                {
                case 0:
                    sceneEditor.Save();
                    sceneEditor.SetVisible(true);
                    sceneEditor.SetScene(sceneIndex);
                    Resources().activeScene = sceneIndex;
                    sceneEditor.entityEditor.SetVisible(false);
                    Hymn().world.Clear();
                    Hymn().world.Load(Hymn().GetPath() + FileSystem::DELIMITER + "Scenes" + FileSystem::DELIMITER + Resources().scenes[sceneIndex] + ".json");
                    changeScene = false;
                    savePromptWindow.SetVisible(false);
                    savePromptWindow.ResetDecision();
                    break;

                case 1:
                    sceneEditor.SetVisible(true);
                    sceneEditor.SetScene(sceneIndex);
                    Resources().activeScene = sceneIndex;
                    sceneEditor.entityEditor.SetVisible(false);
                    Hymn().world.Clear();
                    Hymn().world.Load(Hymn().GetPath() + FileSystem::DELIMITER + "Scenes" + FileSystem::DELIMITER + Resources().scenes[sceneIndex] + ".json");
                    changeScene = false;
                    savePromptWindow.SetVisible(false);
                    savePromptWindow.ResetDecision();
                    break;

                default:
                    break;
                }
            }
        }
    }
    
    // Models.
    bool modelPressed = false;
    if (ImGui::TreeNode("Models")) {
        if (ImGui::Button("Add model")) {
            Geometry::Model* model = new Geometry::Model();
            model->name = "Model #" + std::to_string(Resources().modelNumber++);
            Resources().models.push_back(model);
        }
        
        for (auto it = Resources().models.begin(); it != Resources().models.end(); ++it) {
            Geometry::Model* model = *it;
            if (ImGui::Selectable(model->name.c_str())) {
                modelPressed = true;
                modelEditor.SetModel(model);
            }
            
            if (ImGui::BeginPopupContextItem(model->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    if (modelEditor.GetModel() == model)
                        modelEditor.SetVisible(false);
                    
                    delete model;
                    Resources().models.erase(it);
                    ImGui::EndPopup();
                    break;
                }
                ImGui::EndPopup();
            }
        }
        ImGui::TreePop();
    }
    
    // Textures.
    bool texturePressed = false;
    if (ImGui::TreeNode("Textures")) {
        if (ImGui::Button("Add texture")) {
            TextureAsset* texture = new TextureAsset();
            texture->name = "Texture #" + std::to_string(Resources().textureNumber++);
            Resources().textures.push_back(texture);
        }
        
        for (auto it = Resources().textures.begin(); it != Resources().textures.end(); ++it) {
            TextureAsset* texture = *it;
            if (ImGui::Selectable(texture->name.c_str())) {
                texturePressed = true;
                textureEditor.SetTexture(texture);
            }
            
            if (ImGui::BeginPopupContextItem(texture->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    if (Managers().resourceManager->GetTextureAssetInstanceCount(texture) > 1) {
                        Log() << "This texture is in use. Remove all references to the texture first.\n";
                    } else {
                        if (textureEditor.GetTexture() == texture)
                            textureEditor.SetVisible(false);
                        
                        // Remove files.
                        remove((Hymn().GetPath() + FileSystem::DELIMITER + "Textures" + FileSystem::DELIMITER + texture->name + ".png").c_str());
                        remove((Hymn().GetPath() + FileSystem::DELIMITER + "Textures" + FileSystem::DELIMITER + texture->name + ".json").c_str());
                        
                        Managers().resourceManager->FreeTextureAsset(texture);
                        Resources().textures.erase(it);
                    }
                    ImGui::EndPopup();
                    break;
                }
                ImGui::EndPopup();
            }
        }
        
        ImGui::TreePop();
    }
    
    // Scripts.
    bool scriptPressed = false;
    if (ImGui::TreeNode("Scripts")) {
        if (ImGui::Button("Add script")) {
            ScriptFile* scriptFile = new ScriptFile();
            scriptFile->name = "Script #" + std::to_string(Hymn().scriptNumber++);
            Hymn().scripts.push_back(scriptFile);
        }
        
        for (auto it = Hymn().scripts.begin(); it != Hymn().scripts.end(); ++it) {
            ScriptFile* script = *it;
            std::string name = script->name;
            
            if (ImGui::Selectable(name.c_str())) {
                scriptPressed = true;
                scriptEditor.SetScript(script);
            }
            
            if (ImGui::BeginPopupContextItem(name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    if (scriptEditor.GetScript() == script)
                        scriptEditor.SetVisible(false);
                    
                    delete script;
                    Hymn().scripts.erase(it);
                    ImGui::EndPopup();
                    break;
                }
                ImGui::EndPopup();
            }
        }
        
        ImGui::TreePop();
    }
    
    // Sounds.
    bool soundPressed = false;
    if (ImGui::TreeNode("Sounds")) {
        if (ImGui::Button("Add sound")) {
            Audio::SoundBuffer* sound = new Audio::SoundBuffer();
            sound->name = "Sound #" + std::to_string(Resources().soundNumber++);
            Resources().sounds.push_back(sound);
        }
        
        for (auto it = Resources().sounds.begin(); it != Resources().sounds.end(); ++it) {
            Audio::SoundBuffer* sound = *it;
            if (ImGui::Selectable(sound->name.c_str())) {
                soundPressed = true;
                soundEditor.SetSound(sound);
            }
            
            if (ImGui::BeginPopupContextItem(sound->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    if (soundEditor.GetSound() == sound)
                        soundEditor.SetVisible(false);
                    
                    delete sound;
                    Resources().sounds.erase(it);
                    ImGui::EndPopup();
                    break;
                }
                ImGui::EndPopup();
            }
        }
        
        ImGui::TreePop();
    }
    
    if (sceneEditor.entityPressed || scriptPressed || texturePressed || modelPressed || soundPressed) {
        sceneEditor.entityEditor.SetVisible(sceneEditor.entityPressed);
        scriptEditor.SetVisible(scriptPressed);
        textureEditor.SetVisible(texturePressed);
        modelEditor.SetVisible(modelPressed);
        soundEditor.SetVisible(soundPressed);
    }
    
    if (sceneEditor.IsVisible()) {
        ImGui::HorizontalSplitter(ImVec2(sceneWidth, 20), size.y - 20, splitterSize, sceneWidth, sceneResize, 20, size.x - editorWidth - 20);
        ImGui::SetNextWindowPos(ImVec2(0, 20));
        ImGui::SetNextWindowSize(ImVec2(sceneWidth, size.y - 20));
        sceneEditor.Show();
    }
    
    if (sceneEditor.entityEditor.IsVisible() || scriptEditor.IsVisible() || textureEditor.IsVisible() || modelEditor.IsVisible() || soundEditor.IsVisible()) {
        editorWidth = size.x - editorWidth;
        ImGui::HorizontalSplitter(ImVec2(editorWidth, 20), size.y - 20, splitterSize, editorWidth, editorResize, sceneWidth + 20, size.x - 20);
        editorWidth = size.x - editorWidth;
        
        ImGui::SetNextWindowPos(ImVec2(size.x - editorWidth, 20));
        ImGui::SetNextWindowSize(ImVec2(editorWidth, size.y - 20));
    }
    
    if (sceneEditor.entityEditor.IsVisible())
        sceneEditor.entityEditor.Show();
    if (scriptEditor.IsVisible())
        scriptEditor.Show();
    if (textureEditor.IsVisible())
        textureEditor.Show();
    if (modelEditor.IsVisible())
        modelEditor.Show();
    if (soundEditor.IsVisible())
        soundEditor.Show();
    
    ImGui::End();
}

bool ResourceView::IsVisible() const {
    return visible;
}

void ResourceView::SetVisible(bool visible) {
    this->visible = visible;
}

void ResourceView::HideEditors() {
    sceneEditor.SetVisible(false);
    sceneEditor.entityEditor.SetVisible(false);
    scriptEditor.SetVisible(false);
    modelEditor.SetVisible(false);
    textureEditor.SetVisible(false);
    soundEditor.SetVisible(false);
}

void ResourceView::SaveScene() const {
    sceneEditor.Save();
}

#undef max
void ResourceView::ResetScene() {
    sceneEditor.SetScene(std::numeric_limits<std::size_t>::max());
    sceneEditor.SetVisible(false);
}

SceneEditor& ResourceView::GetScene() {
    return sceneEditor;
}
