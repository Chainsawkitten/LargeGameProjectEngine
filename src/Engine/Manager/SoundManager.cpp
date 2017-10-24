#include "SoundManager.hpp"
#include <phonon.h>
#include <Utility/Log.hpp>
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Listener.hpp"
#include "../Component/SoundSource.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "portaudio.h"
#include <cstdint>
#include <cstring>

#define SAMPLE_RATE (44100)
#define PA_SAMPLE_TYPE paFloat32

SoundManager::SoundManager() {
    PaError err;

    // Initialize PortAudio
    err = Pa_Initialize();
    CheckError(err);

    PaStreamParameters outputParams;

    outputParams.device = Pa_GetDefaultOutputDevice();
    if (outputParams.device >= 0) {
        outputParams.channelCount = 1;
        outputParams.sampleFormat = PA_SAMPLE_TYPE;
        outputParams.hostApiSpecificStreamInfo = NULL;
        outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultHighOutputLatency;
    }

    // Open Stream
    err = Pa_OpenStream(
        &stream,
        NULL,
        &outputParams,
        SAMPLE_RATE,
        paFramesPerBufferUnspecified,
        paClipOff,
        NULL,
        NULL);
    CheckError(err);

    processedFrameSamples = new float[1]{ 0 };

    err = Pa_StartStream(stream);
    CheckError(err);
}

SoundManager::~SoundManager() {
    delete processedFrameSamples;
    Pa_CloseStream(stream);
    Pa_Terminate();
}

void SoundManager::CheckError(PaError err) {
    if (err != paNoError) {
        Pa_Terminate();
        Log() << "An error occured while using the portaudio stream\n";
        Log() << "Error number:" << err << "\n";
        Log() << "Error message: " << Pa_GetErrorText(err) << "\n";
    }
}

void SoundManager::Update(float deltaTime) {
    // Number of samples to process dependant on deltaTime
    int numSamples = int(SAMPLE_RATE * deltaTime);

    // Update sound sources.
    for (Component::SoundSource* sound : soundSources.GetAll()) {
        if (sound->shouldPlay) {
            float* soundBuf = new float[numSamples];
            if (sound->soundBuffer->GetSize() > sound->place + numSamples) {
                std::memcpy(soundBuf, (sound->soundBuffer->GetBuffer() + sound->place), sizeof(float) * numSamples);
                sound->place += numSamples;
            } else {
                // Only copy the end samples of the buffer
                uint32_t numToCpy = numSamples - (sound->soundBuffer->GetSize() - sound->place) / sizeof(float);
                std::memcpy(soundBuf, (sound->soundBuffer->GetBuffer() + sound->place), numToCpy);
                if (sound->loop) {
                    std::memcpy(soundBuf + numToCpy * sizeof(float), sound->soundBuffer->GetBuffer(), sizeof(float) * numSamples - numToCpy);
                    sound->place = numSamples - numToCpy;
                } else {
                    std::memset(soundBuf + numToCpy * sizeof(float), 0, sizeof(float) * (numSamples - numToCpy));
                    sound->shouldPlay = false;
                }
            }

            sAudio.Process(soundBuf, numSamples, 0, 0);
        }

        // Pause it.
        if (sound->shouldPause) {
            sound->shouldPlay = false;
        }

        // Stop it.
        if (sound->shouldStop) {
            sound->shouldPlay = false;
            sound->place = 0;
        }
    }

    uint32_t* numProcessedSamples = new uint32_t;
    float* processedSamples = sAudio.GetProcessed(numProcessedSamples);

    //If not playing anything, add silence
    if (*numProcessedSamples == 0)
        processedSamples = new float[numSamples]{ 0 };

    Pa_WriteStream(stream, processedSamples, *numProcessedSamples);
    delete[] processedSamples;
}

Component::SoundSource* SoundManager::CreateSoundSource() {
    return soundSources.Create();
}

Component::SoundSource* SoundManager::CreateSoundSource(const Json::Value& node) {
    Component::SoundSource* soundSource = soundSources.Create();

    // Load values from Json node.
    std::string name = node.get("sound", "").asString();
    if (!name.empty())
        soundSource->soundBuffer = Managers().resourceManager->CreateSound(name);

    soundSource->pitch = node.get("pitch", 1.f).asFloat();
    soundSource->gain = node.get("gain", 1.f).asFloat();
    soundSource->loop = node.get("loop", false).asBool();

    return soundSource;
}

const std::vector<Component::SoundSource*>& SoundManager::GetSoundSources() const {
    return soundSources.GetAll();
}

Component::Listener* SoundManager::CreateListener() {
    return listeners.Create();
}

Component::Listener* SoundManager::CreateListener(const Json::Value& node) {
    return listeners.Create();
}

const std::vector<Component::Listener*>& SoundManager::GetListeners() const {
    return listeners.GetAll();
}

void SoundManager::ClearKilledComponents() {
    soundSources.ClearKilled();
    listeners.ClearKilled();
}
