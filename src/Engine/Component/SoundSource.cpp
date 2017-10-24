#include "SoundSource.hpp"

#include "../Entity/Entity.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"

using namespace Component;

SoundSource::SoundSource() {
}

SoundSource::~SoundSource() {
    if (soundBuffer != nullptr)
        Managers().resourceManager->FreeSound(soundBuffer);
}

Json::Value SoundSource::Save() const {
    Json::Value component;

    if (soundBuffer != nullptr)
        component["sound"] = soundBuffer->path + soundBuffer->name;

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
