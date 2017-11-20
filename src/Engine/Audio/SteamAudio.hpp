#pragma once
#include "SteamAudioRenderers.hpp"
#include <vector>
#include <cstdint>
#include "../linking.hpp"

//Note to self: Might just merge this shit with the interface class.
/// Main Steam Audio class for processing audio samples
class SteamAudio {
    public:

        ENGINE_API SteamAudio();

        ENGINE_API ~SteamAudio();

        /// Sound source data structure used for Steam Audio.
        struct SoundSourceInfo {
            /// World position of the sound source.
            IPLVector3 position;
            /// Radius sound source for calculating occlusion. (in meters),
            float radius;
            /// Steam Audio buffer containing raw sound data.
            IPLAudioBuffer buffer;
        };

        /// Processes the audio with both indirect and direct methods and mixes them together.
        /**
         * @param inputs Vector of sound sources to be processed.
         * @param output The final processed sound.
         */
        ENGINE_API void Process(std::vector<SteamAudio::SoundSourceInfo>& inputs, IPLAudioBuffer& output);

        /// Sets the current location and direction of the player, to be used for processing
        /**
         * @param pos Vector representing the player's position in world space.
         * @param dir Vector representing the player's forward direction.
         * @param up Vector representing the player's up direction.
         */
        ENGINE_API void SetPlayer(IPLVector3 pos, IPLVector3 dir, IPLVector3 up);

        /// Creates the direct and indirect renderer.
        /**
         * @param environment Handle to a created environment.
         */
        ENGINE_API void CreateRenderers(IPLhandle environment);

    private:
        IPLContext* context;

        SteamAudioRenderers* renderers;

        IPLVector3 playerPos;
        IPLVector3 playerDir;
        IPLVector3 playerUp;
};
