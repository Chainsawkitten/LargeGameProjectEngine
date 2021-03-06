#include "TextureEditor.hpp"

#include <Engine/Texture/TextureAsset.hpp>
#include <Video/Texture/TextureHCT.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>
#include "../../Util/TextureConverter.hpp"

using namespace GUI;

TextureEditor::TextureEditor() {
    name[0] = '\0';
}

void TextureEditor::Show() {
    if (ImGui::Begin(("Texture: " + texture->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(texture))).c_str(), &visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders)) {
        if (ImGui::InputText("Name", name, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
            // Rename texture files.
            std::string path = Hymn().GetPath() + "/" + texture->path;
            rename((path + texture->name + ".hct").c_str(), (path + name + ".hct").c_str());
            
            texture->name = name;
        }
        
        if (texture->GetTexture()->IsLoaded()) {
            ImGui::Image((void*) texture->GetTexture()->GetTextureID(), ImVec2(128, 128));
        } else {
            ImGui::Text("Not loaded");
        }
        
        if (ImGui::Button("Load PNG image")) {
            fileSelector.AddExtensions("png");
            fileSelector.SetInitialPath(Hymn().GetPath().c_str());
            fileSelector.SetFileSelectedCallback(std::bind(&TextureEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }
        
        if (selected) {
            const char* items[3] = { "BC1", "BC4", "BC5" };
            ImGui::Combo("Compression", &compressionType, items, 3);
            
            switch (compressionType) {
            case Video::TextureHCT::BC1:
                ImGui::Text("Suitable for albedo textures.\n4 bits per pixel.");
                break;
            case Video::TextureHCT::BC4:
                ImGui::Text("Suitable for metallic and roughness textures.\n4 bits per pixels.");
                break;
            case Video::TextureHCT::BC5:
                ImGui::Text("Suitable for normal maps.\n8 bits per pixel.");
                break;
            }
            
            if (ImGui::Button("Import")) {
                std::string destination = Hymn().GetPath() + "/" + texture->path + texture->name + ".hct";
                
                // Convert PNG texture to custom texture format.
                TextureConverter::Convert(path.c_str(), destination.c_str(), static_cast<Video::TextureHCT::CompressionType>(compressionType));
                
                texture->Load(texture->path + texture->name);
            }
        }
    }
    ImGui::End();
    
    if (fileSelector.IsVisible())
        fileSelector.Show();
}

const TextureAsset* TextureEditor::GetTexture() const {
    return texture;
}

void TextureEditor::SetTexture(TextureAsset* texture) {
    this->texture = texture;
    selected = false;
    
    strcpy(name, texture->name.c_str());
}

bool TextureEditor::IsVisible() const {
    return visible;
}

void TextureEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void TextureEditor::FileSelected(const std::string& file) {
    path = file;
    selected = true;
}
