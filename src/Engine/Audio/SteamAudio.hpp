#pragma once
#include "SteamAudioDirectRenderer.hpp"
#include "SteamAudioIndirectRenderer.hpp"
#include <vector>
#include <cstdint>
#include "../linking.hpp"

//Note to self: Might just merge this shit with the interface class.
/// Main Steam Audio class for processing audio samples
class SteamAudio {
    public:
    ENGINE_API SteamAudio();

    /**
        * @param context The Steam Audio context.
        * @param environment Handle to the Environment object to use.
        */
    ENGINE_API SteamAudio(IPLContext* context, IPLhandle* environment);

    ENGINE_API ~SteamAudio();

    /// Processes the audio with both indirect and direct methods and mixes them together.
    /**
        * @param input The audiobuffer to be processed.
        * @param playerPos The position of the player in world space.
        * @param playerDir The forward direction of the player.
        * @param playerUp The up direction of the player.
        * @param sourcePos The position of the audio source.
        * @param sourceRadius The radius of the source, for calculating occlusion.
        */
    ENGINE_API void Process(IPLAudioBuffer input, IPLVector3* playerPos, IPLVector3* playerDir, IPLVector3* playerUp, IPLVector3* sourcePos, float sourceRadius);

    /// Mixes and returns the final buffer, ready to be played.
    /**
         * @param finalBuf Pointer to an empty buffer.
         * @param numSamples The number of samples in the final buffer.
         */
    ENGINE_API void GetFinalMix(IPLAudioBuffer* finalBuf, uint32_t* numSamples);

    private:
    IPLAudioBuffer MixAudio(IPLAudioBuffer direct, IPLAudioBuffer indirect);

    std::vector<IPLAudioBuffer> processedBuffers;

    IPLContext* context;

    SteamAudioDirectRenderer directRenderer;
    SteamAudioIndirectRenderer indirectRenderer;
    IPLhandle* environmentalRenderer;
};
