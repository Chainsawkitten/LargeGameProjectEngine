#include "SteamAudioRenderers.hpp"
#include <assert.h>

using namespace Audio;

SteamAudioRenderers::SteamAudioRenderers(IPLhandle environment, IPLhandle envRenderer, IPLhandle binauralRenderer) {
    this->environment = environment;

    inputFormat.channelLayoutType = IPL_CHANNELLAYOUTTYPE_SPEAKERS;
    inputFormat.channelLayout = IPL_CHANNELLAYOUT_MONO; //Only take mono input
    inputFormat.numSpeakers = 1;
    inputFormat.speakerDirections = NULL;
    inputFormat.channelOrder = IPL_CHANNELORDER_INTERLEAVED;

    outputFormat.channelLayoutType = IPL_CHANNELLAYOUTTYPE_SPEAKERS;
    outputFormat.channelLayout = IPL_CHANNELLAYOUT_STEREO;
    outputFormat.numSpeakers = 2;
    outputFormat.speakerDirections = NULL;
    outputFormat.channelOrder = IPL_CHANNELORDER_INTERLEAVED;

    // Direct Sound Effect.
    IPLerror err = iplCreateDirectSoundEffect(envRenderer, inputFormat, inputFormat, &directEffect);
    assert(err == IPL_STATUS_SUCCESS);

    // Binaural Renderer and Effect.
    err = iplCreateBinauralEffect(binauralRenderer, inputFormat, outputFormat, &binauralEffect);
    assert(err == IPL_STATUS_SUCCESS);

    // Convolution Effect
    err = iplCreateConvolutionEffect(envRenderer, "", IPL_SIMTYPE_REALTIME, inputFormat, outputFormat, &convEffect);
    assert(err == IPL_STATUS_SUCCESS);
    
    effectBuffer.format = inputFormat;
    effectBuffer.numSamples = CHUNK_SIZE;
    effectBuffer.interleavedBuffer = new float[CHUNK_SIZE];
    effectBuffer.deinterleavedBuffer = NULL;

    directBuffer.format = outputFormat;
    directBuffer.numSamples = CHUNK_SIZE;
    directBuffer.interleavedBuffer = new float[CHUNK_SIZE * 2];
    directBuffer.deinterleavedBuffer = NULL;
}

SteamAudioRenderers::~SteamAudioRenderers() {
    iplDestroyDirectSoundEffect(&directEffect);
    iplDestroyBinauralEffect(&binauralEffect);
    iplDestroyConvolutionEffect(&convEffect);

    delete[] effectBuffer.interleavedBuffer;
    delete[] directBuffer.interleavedBuffer;
}

IPLAudioBuffer SteamAudioRenderers::Process(IPLAudioBuffer input, IPLVector3 playerPos, IPLVector3 playerDir, IPLVector3 playerUp, IPLVector3 sourcePos, float sourceRadius) {
    // Calculate direct sound path
    IPLDirectSoundPath soundPath = iplGetDirectSoundPath(environment, playerPos, playerDir, playerUp, sourcePos, sourceRadius, IPL_DIRECTOCCLUSION_TRANSMISSIONBYFREQUENCY, IPL_DIRECTOCCLUSION_VOLUMETRIC);

    // Calculate direct effect (occlusion & attenuation)
    IPLDirectSoundEffectOptions options;
    options.applyAirAbsorption = (IPLbool)true;
    options.applyDistanceAttenuation = (IPLbool)true;
    options.directOcclusionMode = IPL_DIRECTOCCLUSION_TRANSMISSIONBYFREQUENCY;

    iplApplyDirectSoundEffect(directEffect, input, soundPath, options, effectBuffer);

    // Spatialize the direct audio
    iplApplyBinauralEffect(binauralEffect, effectBuffer, soundPath.direction, IPL_HRTFINTERPOLATION_BILINEAR, directBuffer);

    // Indirect Audio
    iplSetDryAudioForConvolutionEffect(convEffect, sourcePos, input);

    return directBuffer;
}

void Audio::SteamAudioRenderers::Flush() {
    if (binauralEffect)
        iplFlushBinauralEffect(binauralEffect);
    if (convEffect)
        iplFlushConvolutionEffect(convEffect);
    if (directEffect)
        iplFlushDirectSoundEffect(directEffect);
}
