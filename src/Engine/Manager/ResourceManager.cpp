#include "ResourceManager.hpp"

#include <Video/Geometry/Rectangle.hpp>
#include "../Geometry/Cube.hpp"
#include "../Geometry/Model.hpp"
#include <Video/Texture/Texture2D.hpp>
#include "../Audio/SoundBuffer.hpp"
#include "../Texture/TextureAsset.hpp"
#include "../Script/ScriptFile.hpp"

using namespace std;

ResourceManager::ResourceManager() {
    
}

Geometry::Model* ResourceManager::CreateModel(const std::string& name) {
    if (models.find(name) == models.end()) {
        Geometry::Model* model = new Geometry::Model();
        model->Load(name);
        models[name].model = model;
        modelsInverse[model] = name;
        models[name].count = 1;
    } else {
        models[name].count++;
    }

    return models[name].model;
}

void ResourceManager::FreeModel(Geometry::Model* model) {
    string name = modelsInverse[model];
    
    if (models[name].count-- <= 1) {
        modelsInverse.erase(model);
        delete model;
        models.erase(name);
    }
}

Video::Texture2D* ResourceManager::CreateTexture2D(const char* data, int dataLength, bool srgb) {
    if (textures.find(data) == textures.end()) {
        textures[data].texture = new Video::Texture2D(data, dataLength, srgb);
        texturesInverse[textures[data].texture] = data;
        textures[data].count = 1;
    } else {
        textures[data].count++;
    }
    
    return textures[data].texture;
}

void ResourceManager::FreeTexture2D(Video::Texture2D* texture) {
    const char* data = texturesInverse[texture];
    
    if (textures[data].count-- <= 1) {
        texturesInverse.erase(texture);
        delete texture;
        textures.erase(data);
    }
}

TextureAsset* ResourceManager::CreateTextureAsset(const std::string& name) {
    if (textureAssets.find(name) == textureAssets.end()) {
        TextureAsset* textureAsset = new TextureAsset();
        textureAsset->Load(name);
        textureAssets[name].textureAsset = textureAsset;
        textureAssetsInverse[textureAsset] = name;
        textureAssets[name].count = 1;
    } else {
        textureAssets[name].count++;
    }
    
    return textureAssets[name].textureAsset;
}

void ResourceManager::FreeTextureAsset(TextureAsset* textureAsset) {
    std::string name = textureAssetsInverse[textureAsset];
    
    if (textureAssets[name].count-- <= 1) {
        textureAssetsInverse.erase(textureAsset);
        delete textureAsset;
        textureAssets.erase(name);
    }
}

int ResourceManager::GetTextureAssetInstanceCount(TextureAsset* textureAsset) {
    std::string name = textureAssetsInverse[textureAsset];
    return textureAssets[name].count;
}

Audio::SoundBuffer* ResourceManager::CreateSound(const string& name) {
    if (sounds.find(name) == sounds.end()) {
        Audio::SoundBuffer* soundBuffer = new Audio::SoundBuffer();
        soundBuffer->Load(name);
        sounds[name].soundBuffer = soundBuffer;
        soundsInverse[soundBuffer] = name;
        sounds[name].count = 1;
    } else {
        sounds[name].count++;
    }
    
    return sounds[name].soundBuffer;
}

void ResourceManager::FreeSound(Audio::SoundBuffer* soundBuffer) {
    string name = soundsInverse[soundBuffer];
    
    if (sounds[name].count-- <= 1) {
        soundsInverse.erase(soundBuffer);
        delete soundBuffer;
        sounds.erase(name);
    }
}

ScriptFile* ResourceManager::CreateScriptFile(const string& name) {
    if (scriptFiles.find(name) == scriptFiles.end()) {
        ScriptFile* scriptFile = new ScriptFile();
        scriptFile->Load(name);
        scriptFiles[name].scriptFile = scriptFile;
        scriptFilesInverse[scriptFile] = name;
        scriptFiles[name].count = 1;
    } else {
        scriptFiles[name].count++;
    }
    
    return scriptFiles[name].scriptFile;
}

void ResourceManager::FreeScriptFile(ScriptFile* scriptFile) {
    string name = scriptFilesInverse[scriptFile];
    
    if (scriptFiles[name].count-- <= 1) {
        scriptFilesInverse.erase(scriptFile);
        delete scriptFile;
        scriptFiles.erase(name);
    }
}
