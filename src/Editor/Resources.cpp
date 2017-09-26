#include "Resources.hpp"

#include <json/value.h>
#include <Engine/Texture/TextureAsset.hpp>
#include <Engine/Geometry/Model.hpp>
#include <Engine/Audio/SoundBuffer.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Util/FileSystem.hpp>

using namespace std;

ResourceList::ResourceList() {
    
}

ResourceList& ResourceList::GetInstance() {
    static ResourceList resourceList;
    
    return resourceList;
}

void ResourceList::Save() const {
    Json::Value root;
    
    root["activeScene"] = activeScene;
    
    // Save textures.
    Json::Value texturesNode;
    for (TextureAsset* texture : textures) {
        texturesNode.append(texture->name);
        texture->Save();
    }
    root["textures"] = texturesNode;
    
    // Save models.
    Json::Value modelsNode;
    for (Geometry::Model* model : models) {
        modelsNode.append(model->name);
    }
    root["models"] = modelsNode;
    
    // Save sounds.
    Json::Value soundsNode;
    for (Audio::SoundBuffer* sound : sounds) {
        soundsNode.append(sound->name);
    }
    root["sounds"] = soundsNode;
    
    // Save scenes.
    Json::Value scenesNode;
    for (const string& scene : scenes) {
        scenesNode.append(scene);
    }
    root["scenes"] = scenesNode;
    
    // Save to file.
    ofstream file(Hymn().GetPath() + FileSystem::DELIMITER + "Resources.json");
    file << root;
    file.close();
}

void ResourceList::Load() {
    // Load Json document from file.
    Json::Value root;
    ifstream file(Hymn().GetPath() + FileSystem::DELIMITER + "Resources.json");
    file >> root;
    file.close();
    
    activeScene = root["activeScene"].asUInt();
    
    // Load textures.
    const Json::Value texturesNode = root["textures"];
    for (unsigned int i = 0; i < texturesNode.size(); ++i) {
        textures.push_back(Managers().resourceManager->CreateTextureAsset(texturesNode[i].asString()));
    }
    
    // Load models.
    const Json::Value modelsNode = root["models"];
    for (unsigned int i = 0; i < modelsNode.size(); ++i) {
        models.push_back(Managers().resourceManager->CreateModel(modelsNode[i].asString()));
    }
    
    // Load sounds.
    const Json::Value soundsNode = root["sounds"];
    for (unsigned int i = 0; i < soundsNode.size(); ++i) {
        sounds.push_back(Managers().resourceManager->CreateSound(soundsNode[i].asString()));
    }
    
    // Load scenes.
    const Json::Value scenesNode = root["scenes"];
    for (unsigned int i = 0; i < scenesNode.size(); ++i) {
        scenes.push_back(scenesNode[i].asString());
    }
    
    textureNumber = textures.size();
    modelNumber = models.size();
    soundNumber = sounds.size();
}

void ResourceList::Clear() {
    scenes.clear();
    
    for (Geometry::Model* model : models) {
        Managers().resourceManager->FreeModel(model);
    }
    models.clear();
    modelNumber = 0U;
    
    for (TextureAsset* texture : textures) {
        Managers().resourceManager->FreeTextureAsset(texture);
    }
    textures.clear();
    textureNumber = 0U;
    
    for (Audio::SoundBuffer* sound : sounds) {
        Managers().resourceManager->FreeSound(sound);
    }
    sounds.clear();
    soundNumber = 0U;
}

ResourceList& Resources() {
    return ResourceList::GetInstance();
}
