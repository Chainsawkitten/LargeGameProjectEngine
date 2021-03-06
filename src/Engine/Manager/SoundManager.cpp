#include "SoundManager.hpp"
#include <phonon.h>
#include <Utility/Log.hpp>
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/AudioMaterial.hpp"
#include "../Component/Listener.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/SoundSource.hpp"
#include "../Audio/SoundFile.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "../Audio/SoundStreamer.hpp"
#include "../Audio/AudioMaterial.hpp"
#include <Video/Geometry/Geometry3D.hpp>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include <portaudio.h>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <GLFW/glfw3.h>

using namespace Audio;

SoundManager::SoundManager() {
    PaError err;

    // Initialize PortAudio
    err = Pa_Initialize();
    CheckError(err);

    PaStreamParameters outputParams;

    outputParams.device = Pa_GetDefaultOutputDevice();
    if (outputParams.device >= 0) {
        outputParams.channelCount = 2;
        outputParams.sampleFormat = paFloat32;
        outputParams.hostApiSpecificStreamInfo = NULL;
        outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultHighOutputLatency;
    }

    // Open Stream
    err = Pa_OpenStream(
        &stream,
        NULL,
        &outputParams,
        SAMPLE_RATE,
        CHUNK_SIZE,
        0,
        PortAudioStreamCallback,
        this
    );
    CheckError(err);

    err = Pa_StartStream(stream);
    CheckError(err);

    memset(processedBuffer, 0, sizeof(float) * CHUNK_SIZE * 2);
}


SoundManager::~SoundManager() {
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

void SoundManager::CheckStatusFlag(PaStreamCallbackFlags flags) {
    if (flags == 0)
        return;

    std::string str = "";
    if (flags & paInputUnderflow)
        str += "InputUnderflow; ";
    if (flags & paInputOverflow)
        str += "InputOverflow; ";
    if (flags & paOutputUnderflow)
        str += "OutputUnderflow; ";
    if (flags & paOutputOverflow)
        str += "OutputOverflow; ";
    if (flags & paPrimingOutput)
        str += "PrimingOutput; ";

    Log() << "PaStream Callback Status: " << str << "\n";
}

int SoundManager::PortAudioStreamCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    SoundManager* soundManager = (SoundManager*)userData;

    std::unique_lock<std::mutex> updateLock(soundManager->updateMutex, std::defer_lock);
    updateLock.lock();
    const std::vector<Component::Listener*>& listeners = soundManager->GetListeners();
    if (listeners.size() > 0) {
        CheckStatusFlag(statusFlags);
        assert(framesPerBuffer == CHUNK_SIZE);
        memcpy(outputBuffer, soundManager->processedBuffer, sizeof(float) * CHUNK_SIZE * 2);
        soundManager->ProcessSamples();
    }
    updateLock.unlock();

    return paContinue;
}

void SoundManager::ProcessSamples() {
    const std::vector<Component::Listener*>& listeners = GetListeners();
    assert(listeners.size() > 0);
  
    Entity* player = listeners[0]->entity;
    glm::vec3 glmPos = player->GetWorldPosition();
    glm::quat orientation = player->GetWorldOrientation();
    glm::vec3 glmDir = orientation * glm::vec3(0, 0, -1);
    glm::vec3 glmUp = orientation * glm::vec3(0, 1, 0);
    IPLVector3 pos = { glmPos.x, glmPos.y, glmPos.z };
    IPLVector3 dir = { glmDir.x, glmDir.y, glmDir.z };
    IPLVector3 up = { glmUp.x, glmUp.y, glmUp.z };

    // Set player transform
    sAudio.SetPlayer(pos, dir, up);

    std::vector<SoundBuffer*> soundBuffers;
    std::vector<float*> buffers;
    std::vector<IPLVector3> positions;
    std::vector<float> radii;
    std::vector<SteamAudioRenderers*> renderers;

    // Update sound sources.
    for (Component::SoundSource* sound : soundSources.GetAll()) {
        Audio::SoundBuffer* soundBuffer = sound->soundBuffer;
        Audio::SoundFile* soundFile = soundBuffer->GetSoundFile();

        // Check if sound should play and is a valid resource.
        if (sound->shouldPlay && soundFile && soundFile->IsLoaded()) {

            // Get samples from streamed buffer.
            int samples;
            float* buffer = soundBuffer->GetChunkData(samples);
            if (buffer) {
                buffers.push_back(buffer);
                soundBuffers.push_back(soundBuffer);

                // Volume.
                for (int m = 0; m < samples; ++m)
                    buffer[m] *= sound->volume;

                glm::vec3 position = sound->entity->GetWorldPosition();
                positions.push_back(IPLVector3{ position.x, position.y, position.z });
                radii.push_back(3.0f);
                if (!sound->renderers)
                    sAudio.CreateRenderers(sound->renderers);
                renderers.push_back(sound->renderers);
            }

            // If end of file, check if sound repeat.
            if (samples < CHUNK_SIZE) {
                soundBuffer->Restart();
                sound->shouldStop = !sound->loop;
                // Set silence (zero) at end of buffer.
                if (buffer)
                    memset(&buffer[samples], 0, (CHUNK_SIZE - samples) * sizeof(float));
            }
        }
    }

    // Process sound.
    if (soundBuffers.empty())
        memset(processedBuffer, 0, CHUNK_SIZE * 2 * sizeof(float));
    else
        sAudio.Process(buffers, positions, radii, renderers, processedBuffer);

    // Consume used chunk and produce new chunk.
    for (Audio::SoundBuffer* soundBuffer : soundBuffers) {
        soundBuffer->ConsumeChunk();
        soundBuffer->ProduceChunk();
    }

    for (Component::SoundSource* sound : soundSources.GetAll()) {
        // Pause it.
        if (sound->shouldPause) {
            sound->shouldPlay = false;
            if (sound->renderers) {
                sound->renderers->Flush();
                delete sound->renderers;
                sound->renderers = nullptr;
            }
        }

        // Stop it.
        if (sound->shouldStop) {
            sound->soundBuffer->Restart();
            sound->shouldPlay = false;
            if (sound->renderers) {
                sound->renderers->Flush();
                delete sound->renderers;
                sound->renderers = nullptr;
            }
        }
    }
}

Component::SoundSource* SoundManager::CreateSoundSource() {
    std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
    updateLock.lock();
    Component::SoundSource* soundSource = soundSources.Create();
    updateLock.unlock();
    return soundSource;
}

Component::SoundSource* SoundManager::CreateSoundSource(const Json::Value& node) {
    std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
    updateLock.lock();
    Component::SoundSource* soundSource = soundSources.Create();

    // Load values from Json node.
    std::string name = node.get("sound", "").asString();
    if (!name.empty())
        soundSource->soundBuffer->SetSoundFile(Managers().resourceManager->CreateSound(name));

    soundSource->volume = node.get("volume", 1.f).asFloat();
    soundSource->loop = node.get("loop", false).asBool();

    updateLock.unlock();
    return soundSource;
}

const std::vector<Component::SoundSource*>& SoundManager::GetSoundSources() const {
    return soundSources.GetAll();
}

Component::Listener* SoundManager::CreateListener() {
    std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
    updateLock.lock();
    Component::Listener* listener = listeners.Create();
    updateLock.unlock();
    return listener;
}

Component::Listener* SoundManager::CreateListener(const Json::Value& node) {
    std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
    updateLock.lock();
    Component::Listener* listener = listeners.Create();
    updateLock.unlock();
    return listener;
}

const std::vector<Component::Listener*>& SoundManager::GetListeners() const {
    return listeners.GetAll();
}

Component::AudioMaterial* SoundManager::CreateAudioMaterial() {
    std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
    updateLock.lock();
    Component::AudioMaterial* audioMaterial = audioMaterials.Create();
    updateLock.unlock();
    return audioMaterial;
}

Component::AudioMaterial* SoundManager::CreateAudioMaterial(const Json::Value& node) {
    std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
    updateLock.lock();
    Component::AudioMaterial* audioMaterial = audioMaterials.Create();

    // Load values from Json node.
    std::string name = node.get("audio material", "").asString();
    if (!name.empty())
        audioMaterial->material = Managers().resourceManager->CreateAudioMaterial(name);

    updateLock.unlock();
    return audioMaterial;
}

const std::vector<Component::AudioMaterial*>& SoundManager::GetAudioMaterials() const {
    return audioMaterials.GetAll();
}

void SoundManager::CreateAudioEnvironment() {
    std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
    updateLock.lock();
    // Temporary list of all audio materials in use
    std::vector<Audio::AudioMaterial*> audioMatRes;

    int numMaterials = 0;
    // Get all material resources in use
    for (const Component::AudioMaterial* audioMatComp : GetAudioMaterials()) {

        std::vector<Audio::AudioMaterial*>::iterator it;
        it = std::find(audioMatRes.begin(), audioMatRes.end(), audioMatComp->material);
        // Add the resource if it's not already in the list
        if (it == audioMatRes.end()) {
            audioMatRes.push_back(audioMatComp->material);
            numMaterials++;
        }
    }

    // Create Scene
    sAudio.CreateScene(static_cast<uint32_t>(audioMatRes.size()));

    for (std::size_t i = 0; i < audioMatRes.size(); ++i) {
        IPLMaterial iplmat;
        iplmat.highFreqAbsorption = audioMatRes[i]->highFreqAbsorption;
        iplmat.midFreqAbsorption = audioMatRes[i]->midFreqAbsorption;
        iplmat.lowFreqAbsorption = audioMatRes[i]->lowFreqAbsorption;
        iplmat.highFreqTransmission = audioMatRes[i]->highFreqTransmission;
        iplmat.midFreqTransmission = audioMatRes[i]->midFreqTransmission;
        iplmat.lowFreqTransmission = audioMatRes[i]->lowFreqTransmission;
        iplmat.scattering = audioMatRes[i]->scattering;

        sAudio.SetSceneMaterial(static_cast<uint32_t>(i), iplmat);
    }

    // Create mesh.
    for (const Component::AudioMaterial* audioMatComp : GetAudioMaterials()) {
        Entity* entity = audioMatComp->entity;
        Component::Mesh* mesh = entity->GetComponent<Component::Mesh>();
        if (mesh && mesh->geometry) {
            const std::vector<glm::vec3>& meshVertices = mesh->geometry->GetVertexPositionData();
            const std::vector<uint32_t>& meshIndices = mesh->geometry->GetVertexIndexData();

            // Create ipl mesh if vertex data is valid.
            if (meshVertices.size() > 0 && meshIndices.size() > 0) {
                const glm::mat4 modelMatrix = entity->GetModelMatrix();
                std::vector<IPLVector3> iplVertices;
                std::vector<IPLTriangle> iplIndices;

                // Convert and transform vertices.
                iplVertices.resize(meshVertices.size());
                for (std::size_t i = 0; i < meshVertices.size(); ++i) {
                    const glm::vec4 transformedVector = modelMatrix * glm::vec4(meshVertices[i], 1.f);
                    iplVertices[i] = IPLVector3{ transformedVector.x, transformedVector.y, transformedVector.z };
                }

                // Convert indices.
                iplIndices.resize(meshIndices.size());
                for (std::size_t i = 0; i < meshIndices.size(); i+=3) {
                    iplIndices[i] = IPLTriangle{ (IPLint32)meshIndices[i], (IPLint32)meshIndices[i+1], (IPLint32)meshIndices[i+2] };
                }

                // Find material index and create ipl mesh.
                for (std::size_t i = 0; i < audioMatRes.size(); ++i) {
                    if (audioMatRes[i] == audioMatComp->material) {
                        sAudio.CreateStaticMesh(iplVertices, iplIndices, static_cast<int>(i));
                        break;
                    }
                }
            }
        }
    }

    sAudio.FinalizeScene(NULL);

    // Create Environment.
    sAudio.CreateEnvironment();
    updateLock.unlock();
}

void SoundManager::ClearKilledComponents() {
    const std::vector<Component::AudioMaterial*>& audioMaterialVector = audioMaterials.GetAll();
    for (std::size_t i = 0; i < audioMaterialVector.size(); ++i)
        if (audioMaterialVector[i]->IsKilled()) {
            std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
            updateLock.lock();
            audioMaterials.ClearKilled();
            updateLock.unlock();
            break;
        }
    const std::vector<Component::SoundSource*>& soundSourceVector = soundSources.GetAll();
    for (std::size_t i = 0; i < soundSourceVector.size(); ++i)
        if (soundSourceVector[i]->IsKilled()) {
            std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
            updateLock.lock();
            soundSources.ClearKilled();
            updateLock.unlock();
            break;
        }
    const std::vector<Component::Listener*>& listenerMaterialVector = listeners.GetAll();
    for (std::size_t i = 0; i < listenerMaterialVector.size(); ++i)
        if (listenerMaterialVector[i]->IsKilled()) {
            std::unique_lock<std::mutex> updateLock(updateMutex, std::defer_lock);
            updateLock.lock();
            listeners.ClearKilled();
            updateLock.unlock();
            break;
        }
}

void SoundManager::Load(SoundStreamer::DataHandle* handle) {
    if (handle->soundFile->IsCached()) {
        handle->samples = handle->soundFile->GetData(handle->offset, handle->samples, handle->data);
        handle->done = true;
    } else
        soundStreamer.Load(handle);
}

void SoundManager::Flush(Utility::Queue<SoundStreamer::DataHandle>& queue, bool lock) {
    if (lock)
        soundStreamer.BeginFlush();
    while (SoundStreamer::DataHandle* handle = queue.Iterate()) {
        handle->abort = true;
        if (handle->soundFile->IsCached())
            handle->done = true;
    }
    if (lock)
        soundStreamer.EndFlush();
}
