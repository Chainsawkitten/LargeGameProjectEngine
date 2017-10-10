#include "TextureAsset.hpp"

#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"
#include <Video/Texture/Texture2D.hpp>
#include <fstream>

using namespace Video;

TextureAsset::TextureAsset() {
    texture = new Texture2D();
}

TextureAsset::~TextureAsset() {
    delete texture;
}

void TextureAsset::Save() const {
    Json::Value texture;
    texture["srgb"] = srgb;
    
    // Save properties to meta file.
    std::string filename = Hymn().GetPath() + "/" + path + name + ".json";
    std::ofstream file(filename);
    file << texture;
    file.close();
}

void TextureAsset::Load(const std::string& name) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);
    std::string filename = Hymn().GetPath() + "/" + name;
    
    // Get properties from meta file.
    Json::Value root;
    std::ifstream file(filename + ".json");
    file >> root;
    file.close();
    
    srgb = root.get("srgb", false).asBool();
    
    // Load texture from disk.
    texture->Load((filename + ".png").c_str(), srgb);
}

void TextureAsset::Load(const std::string& name, Video::Texture2D* texture, bool srgb) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);
    this->srgb = srgb;

    this->texture = texture;
}

Texture2D* TextureAsset::GetTexture() const {
    return texture;
}
