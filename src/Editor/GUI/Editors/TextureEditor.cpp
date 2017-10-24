#include "TextureEditor.hpp"

#include <Engine/Texture/TextureAsset.hpp>
#include <Video/Texture/Texture2D.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;

TextureEditor::TextureEditor() {
    name[0] = '\0';
}

void TextureEditor::Show() {
    if (ImGui::Begin(("Texture: " + texture->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(texture))).c_str(), &visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders)) {
        ImGui::InputText("Name", name, 128);
        texture->name = name;

        if (texture->GetTexture()->IsLoaded()) {
            ImGui::Image((void*)texture->GetTexture()->GetTextureID(), ImVec2(128, 128));
        } else {
            ImGui::Text("Not loaded");
        }

        if (ImGui::Button("Load PNG image")) {
            fileSelector.AddExtensions("png");
            fileSelector.SetFileSelectedCallback(std::bind(&TextureEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }

        ImGui::Checkbox("SRGB", &texture->srgb);
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

    strcpy(name, texture->name.c_str());
}

bool TextureEditor::IsVisible() const {
    return visible;
}

void TextureEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void TextureEditor::FileSelected(const std::string& file) {
    std::string destination = Hymn().GetPath() + "/" + texture->path + texture->name + ".png";
    FileSystem::Copy(file.c_str(), destination.c_str());
    texture->GetTexture()->Load(file.c_str(), texture->srgb);
}
