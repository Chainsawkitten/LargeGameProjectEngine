#include "SoundSource.hpp"

#include "../Entity/Entity.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"

using namespace Component;

SoundSource::SoundSource() {
    alGenSources(1, &source);
}

SoundSource::~SoundSource() {
    alDeleteSources(1, &source);
    
    if (soundBuffer != nullptr)
        Managers().resourceManager->FreeSound(soundBuffer);
}

Json::Value SoundSource::Save() const {
    Json::Value component;
    
    if (soundBuffer != nullptr)
        component["sound"] = soundBuffer->name;
    
    component["pitch"] = pitch;
    component["gain"] = gain;
    component["loop"] = loop;
    return component;
}

void SoundSource::Play() {
    shouldPlay = true;
}

void SoundSource::Pause() {
    shouldPause = true;
}

void SoundSource::Stop() {
    shouldStop = true;
}
