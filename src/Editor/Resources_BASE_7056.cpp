#include "Resources.hpp"

#include <Engine/Texture/TextureAsset.hpp>
#include <Engine/Geometry/Model.hpp>
#include <Engine/Audio/SoundBuffer.hpp>
#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Util/FileSystem.hpp>

using namespace std;

string ResourceList::Resource::GetName() const {
    switch (type) {
    case Type::SCENE:
        return scene;
    case Type::MODEL:
        return model->name;
    case Type::TEXTURE:
        return texture->name;
    case Type::SOUND:
        return sound->name;
    case Type::SCRIPT:
        return script->name;
    default:
        return "";
    }
}

ResourceList::ResourceList() {
    resourceFolder.name = "Resources";
}

ResourceList& ResourceList::GetInstance() {
    static ResourceList resourceList;
    
    return resourceList;
}

void ResourceList::Save() const {
    // Save to file.
    ofstream file(GetSavePath());
    file << ToJson();
    file.close();
}

Json::Value ResourceList::ToJson() const {
    Json::Value root;
    
    root["activeScene"] = activeScene;
    root["resourceFolder"] = SaveFolder(resourceFolder);
    
    root["sceneNumber"] = sceneNumber;
    root["textureNumber"] = textureNumber;
    root["modelNumber"] = modelNumber;
    root["soundNumber"] = soundNumber;
    root["scriptNumber"] = scriptNumber;
    
    return root;
}

void ResourceList::Load() {
    // Load Json document from file.
    Json::Value root;
    ifstream file(GetSavePath());
    file >> root;
    file.close();
    
    activeScene = root["activeScene"].asString();
    resourceFolder = LoadFolder(root["resourceFolder"], "");
    
    sceneNumber = root["sceneNumber"].asUInt();
    textureNumber = root["textureNumber"].asUInt();
    modelNumber = root["modelNumber"].asUInt();
    soundNumber = root["soundNumber"].asUInt();
    scriptNumber = root["scriptNumber"].asUInt();
}

void ResourceList::Clear() {
    ClearFolder(resourceFolder);
    resourceFolder.name = "Resources";
    
    sceneNumber = 0U;
    modelNumber = 0U;
    textureNumber = 0U;
    soundNumber = 0U;
    scriptNumber = 0U;
}

Json::Value ResourceList::SaveFolder(const ResourceFolder& folder) const {
    Json::Value node;
    node["name"] = folder.name;
    
    // Save subfolders.
    Json::Value subfolders;
    for (const ResourceFolder& subfolder : folder.subfolders)
        subfolders.append(SaveFolder(subfolder));
    
    node["subfolders"] = subfolders;
    
    // Save resources.
    Json::Value resourcesNode;
    for (const Resource& resource : folder.resources) {
        Json::Value resourceNode;
        resourceNode["type"] = resource.type;
        
        switch (resource.type) {
        case Resource::SCENE:
            resourceNode["scene"] = resource.scene;
            break;
        case Resource::TEXTURE:
            resourceNode["texture"] = resource.texture->name;
            resource.texture->Save();
            break;
        case Resource::MODEL:
            resourceNode["model"] = resource.model->name;
            resource.model->Save();
            break;
        case Resource::SOUND:
            resourceNode["sound"] = resource.sound->name;
            resource.sound->Save();
            break;
        case Resource::SCRIPT:
            resourceNode["script"] = resource.script->name;
            resource.script->Save();
            break;
        }
        
        resourcesNode.append(resourceNode);
    }
    node["resources"] = resourcesNode;
    
    return node;
}

ResourceList::ResourceFolder ResourceList::LoadFolder(const Json::Value& node, std::string path) {
    ResourceFolder folder;
    folder.name = node["name"].asString();
    path += folder.name + "/";
    
    // Load subfolders.
    Json::Value subfoldersNode = node["subfolders"];
    for (unsigned int i = 0; i < subfoldersNode.size(); ++i)
        folder.subfolders.push_back(LoadFolder(subfoldersNode[i], path));
    
    // Load resources.
    Json::Value resourcesNode = node["resources"];
    for (unsigned int i = 0; i < resourcesNode.size(); ++i) {
        Json::Value resourceNode = resourcesNode[i];
        Resource resource;
        resource.type = static_cast<Resource::Type>(resourceNode["type"].asInt());
        
        switch (resource.type) {
        case Resource::SCENE:
            resource.scene = resourceNode["scene"].asString();
            break;
        case Resource::TEXTURE:
            resource.texture = Managers().resourceManager->CreateTextureAsset(path + resourceNode["texture"].asString());
            break;
        case Resource::MODEL:
            resource.model = Managers().resourceManager->CreateModel(path + resourceNode["model"].asString());
            break;
        case Resource::SOUND:
            resource.sound = Managers().resourceManager->CreateSound(path + resourceNode["sound"].asString());
            break;
        case Resource::SCRIPT:
            resource.script = Managers().resourceManager->CreateScriptFile(path + resourceNode["script"].asString());
            break;
        }
        
        folder.resources.push_back(resource);
    }
    
    return folder;
}

void ResourceList::ClearFolder(ResourceFolder& folder) {
    // Clear subfolders.
    for (ResourceFolder& subfolder : folder.subfolders)
        ClearFolder(subfolder);
    
    folder.subfolders.clear();
    
    // Clear resources.
    for (const Resource& resource : folder.resources) {
        switch (resource.type) {
        case Resource::Type::MODEL:
            Managers().resourceManager->FreeModel(resource.model);
            break;
        case Resource::Type::TEXTURE:
            Managers().resourceManager->FreeTextureAsset(resource.texture);
            break;
        case Resource::Type::SOUND:
            Managers().resourceManager->FreeSound(resource.sound);
            break;
        default:
            break;
        }
    }
    folder.resources.clear();
}

std::string ResourceList::GetSavePath() const{
    return Hymn().GetPath() + FileSystem::DELIMITER + "Resources.json";
}

ResourceList& Resources() {
    return ResourceList::GetInstance();
}
