#include "TextureAsset.hpp"

#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"
#include <Video/Texture/Texture2D.hpp>

using namespace Video;

TextureAsset::TextureAsset() {
    texture = new Texture2D();
}

TextureAsset::~TextureAsset() {
    delete texture;
}

Json::Value TextureAsset::Save() const {
    Json::Value texture;
    texture["name"] = name;
    texture["srgb"] = srgb;
    return texture;
}

void TextureAsset::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    texture->Load((Hymn().GetPath() + FileSystem::DELIMITER + "Textures" + FileSystem::DELIMITER + name + ".png").c_str(), node.get("srgb", false).asBool());
}

Texture2D* TextureAsset::GetTexture() const {
    return texture;
}
