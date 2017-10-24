#include "SteamAudioInterface.hpp"

SteamAudioInterface::SteamAudioInterface() {
    context = nullptr;
    playerDir = new IPLVector3{ 0, 0, 0 };
    playerPos = new IPLVector3{ 0, 0, 0 };
    playerUp = new IPLVector3{ 0, 0, 0 };
}

SteamAudioInterface::~SteamAudioInterface() {
    delete context;
    // Might have to remove these depending on how the pointers are sent to this class
    delete playerDir;
    delete playerPos;
    delete playerUp;
}

void SteamAudioInterface::Process(float* input, uint32_t samples, IPLVector3* sourcePosition, float sourceRadius) {
    //Create a steam audio buffer
    IPLAudioFormat format;
    format.channelLayoutType = IPL_CHANNELLAYOUTTYPE_SPEAKERS;
    format.channelLayout = IPL_CHANNELLAYOUT_MONO; //Only take mono input
    format.numSpeakers = 1;
    format.speakerDirections = NULL;
    format.channelOrder = IPL_CHANNELORDER_INTERLEAVED;

    IPLAudioBuffer buf;
    buf.format = format;
    buf.numSamples = samples;
    buf.interleavedBuffer = input;
    buf.deinterleavedBuffer = NULL;

    sAudio.Process(buf, 0, 0, 0, 0, 0);
}

float* SteamAudioInterface::GetProcessed(uint32_t* numSamples) {
    IPLAudioBuffer* finalBuf = new IPLAudioBuffer;
    sAudio.GetFinalMix(finalBuf, numSamples);

    return finalBuf->interleavedBuffer;
}
