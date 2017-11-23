#pragma once

#include "../Entity/ComponentContainer.hpp"
#include <portaudio.h>
#include "../Audio/SteamAudioInterface.hpp"
#include "../linking.hpp"
#include "../Audio/SoundStreamer.hpp"

#define CHUNK_COUNT (15)

namespace Audio {
    class SoundStreamer;
}

namespace Component {
    class AudioMaterial;
    class Listener;
    class SoundSource;
}
namespace Json {
    class Value;
}

/// Handles OpenAL sound.
class SoundManager {
    friend class Hub;
    
    public:     
        /// Check for OpenAL errors.
        /**
         * @param err The PortAudio error number to check.
         */
        ENGINE_API static void CheckError(PaError err);
        
        /// Moves sound sources and plays sounds.
        /**
         * @param deltaTime Time since last frame.
         */
        ENGINE_API void Update(float deltaTime);
        
        /// Create sound source component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::SoundSource* CreateSoundSource();
        
        /// Create sound source component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::SoundSource* CreateSoundSource(const Json::Value& node);
        
        /// Get all sound source components.
        /**
         * @return All sound source components.
         */
        ENGINE_API const std::vector<Component::SoundSource*>& GetSoundSources() const;
        
        /// Create listener component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::Listener* CreateListener();
        
        /// Create listener component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::Listener* CreateListener(const Json::Value& node);
        
        /// Get all listener components.
        /**
         * @return All listener components.
         */
        ENGINE_API const std::vector<Component::Listener*>& GetListeners() const;

        /// Create audio material component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::AudioMaterial* CreateAudioMaterial();

        /// Create audio material component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::AudioMaterial* CreateAudioMaterial(const Json::Value& node);

        /// Get all audio material components.
        /**
         * @return All audio material components.
         */
        ENGINE_API const std::vector<Component::AudioMaterial*>& GetAudioMaterials() const;

        /// Creates the audio environment Steam Audio uses.
        ENGINE_API void CreateAudioEnvironment();
        
        /// Remove all killed components.
        ENGINE_API void ClearKilledComponents();

        //TMPTODO
        ENGINE_API void Load(Audio::SoundStreamer::DataHandle& dataHandle);

        //TMPTODO
        ENGINE_API void Flush(std::queue<Audio::SoundStreamer::DataHandle>& queue);
        
    private:
        SoundManager();
        ~SoundManager();
        SoundManager(SoundManager const&) = delete;
        void operator=(SoundManager const&) = delete;

        void ProcessSamples();
        
        SteamAudioInterface sAudio;
        PaStream* stream;
        Audio::SoundStreamer soundStreamer;

        unsigned int targetSample = 0;
        unsigned int processedSamples = 0;
        unsigned int currentSample = 0;
        float processedBuffer[CHUNK_SIZE * 2];

        float volume = 1.f;
        
        ComponentContainer<Component::SoundSource> soundSources;
        ComponentContainer<Component::Listener> listeners;
        ComponentContainer<Component::AudioMaterial> audioMaterials;
};
