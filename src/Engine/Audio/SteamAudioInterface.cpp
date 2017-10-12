#include "SteamAudioInterface.hpp"


SteamAudioInterface::SteamAudioInterface() {

}

SteamAudioInterface::~SteamAudioInterface() {

}

void SteamAudioInterface::Process(float* input, uint32_t samples,  IPLVector3 * sourcePosition, float sourceRadius) {

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

float * SteamAudioInterface::GetProcessed(uint32_t* numSamples) {
    IPLAudioBuffer* finalBuf = new IPLAudioBuffer;
    sAudio.GetFinalMix(finalBuf, numSamples);

    return finalBuf->interleavedBuffer;
}
