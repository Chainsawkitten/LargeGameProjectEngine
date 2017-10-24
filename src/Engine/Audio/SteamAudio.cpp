#include "SteamAudio.hpp"

SteamAudio::SteamAudio() {
    context = nullptr;
    environmentalRenderer = nullptr;
}

SteamAudio::SteamAudio(IPLContext* context, IPLhandle* environment) {
}

SteamAudio::~SteamAudio() {
    if (context != nullptr)
        delete context;
}

void SteamAudio::Process(IPLAudioBuffer input, IPLVector3* playerPos, IPLVector3* playerDir, IPLVector3* playerUp, IPLVector3* sourcePos, float sourceRadius) {
    // Direct Processing

    // Indirect Processing

    //MixAudio(direct, indirect)

    processedBuffers.push_back(input);
}

void SteamAudio::GetFinalMix(IPLAudioBuffer* finalBuf, uint32_t* numSamples) {
    // No audio is playing, return emptiness, let SoundManager figure it out.
    if (processedBuffers.size() == 0) {
        *numSamples = 0;
        finalBuf = nullptr;
        return;
    }

    // Move the buffers into an array (needed for steam audios mixing)
    IPLAudioBuffer* buffers = new IPLAudioBuffer[processedBuffers.size()];
    for (int i = 0; i < processedBuffers.size(); i++) {
        buffers[i] = processedBuffers[i];
    }

    finalBuf->format = buffers[0].format;
    finalBuf->interleavedBuffer = new float[buffers[0].numSamples];
    finalBuf->numSamples = buffers[0].numSamples;
    iplMixAudioBuffers(processedBuffers.size(), buffers, *finalBuf);
    *numSamples = finalBuf->numSamples;

    // Clean up
    for (IPLAudioBuffer buf : processedBuffers) {
        if (buf.interleavedBuffer != nullptr)
            delete[] buf.interleavedBuffer;
    }
    processedBuffers.clear();
}
