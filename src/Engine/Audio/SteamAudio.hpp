#pragma once
#include "SteamAudioDirectRenderer.hpp"
#include "SteamAudioIndirectRenderer.hpp"

//Note to self: Might just merge this shit with the interface class.
/// Main Steam Audio class for processing audio samples
class SteamAudio {
    public:

        SteamAudio();

        /**
        * @param context The Steam Audio context.
        * @param environment Handle to the Environment object to use.
        */
        SteamAudio(IPLContext* context, IPLhandle* environment);

        ~SteamAudio();

        /// Processes the audio with both indirect and direct methods and mixes them together.
        /**
        * @param input The audiobuffer to be processed.
        * @param playerPos The position of the player in world space.
        * @param playerDir The forward direction of the player.
        * @param playerUp The up direction of the player.
        * @param sourcePos The position of the audio source.
        * @param sourceRadius The radius of the source, for calculating occlusion.
        * @return The processed buffer.
        */
        IPLAudioBuffer Process(IPLAudioBuffer input, IPLVector3* playerPos, IPLVector3* playerDir, IPLVector3* playerUp, IPLVector3* sourcePos, float sourceRadius);

    private:

        IPLAudioBuffer MixAudio(IPLAudioBuffer direct, IPLAudioBuffer indirect);

        IPLContext* context;

        SteamAudioDirectRenderer directRenderer;
        SteamAudioIndirectRenderer indirectRenderer;
        IPLhandle* environmentalRenderer;
};
