#include "SteamAudioInterface.hpp"
#include <assert.h>

SteamAudioInterface::SteamAudioInterface() {
    simSettings.sceneType = IPL_SCENETYPE_PHONON;
    simSettings.numRays = 12400;
    simSettings.numDiffuseSamples = 1024;
    simSettings.numBounces = 3;
    simSettings.irDuration = 1.0;
    simSettings.ambisonicsOrder = 1;
    simSettings.maxConvolutionSources = 10;
    context = IPLContext{ nullptr, nullptr, nullptr };
}

SteamAudioInterface::~SteamAudioInterface() {
    iplDestroyEnvironment(&environment);
    iplDestroyScene(&scene);
}

void SteamAudioInterface::CreateScene(uint32_t numMaterials) {
    IPLerror err = iplCreateScene(context, NULL, simSettings, numMaterials, &scene);
}

void SteamAudioInterface::FinalizeScene(IPLFinalizeSceneProgressCallback progressCallback) {
    iplFinalizeScene(scene, NULL);
}

SteamAudioInterface::SaveData SteamAudioInterface::SaveFinalizedScene() {
    SaveData saveData;

    // Call once first only to figure out the size of the saved scene.
    saveData.sceneSize = iplSaveFinalizedScene(scene, NULL);

    // Actually save the scene.
    saveData.scene = new IPLbyte[saveData.sceneSize];
    iplSaveFinalizedScene(scene, saveData.scene);

    return saveData;
}

void SteamAudioInterface::LoadFinalizedScene(SaveData data) {
    iplLoadFinalizedScene(context, simSettings, data.scene, data.sceneSize, NULL, NULL, &scene);
}

void SteamAudioInterface::SetSceneMaterial(uint32_t matIndex, IPLMaterial material) {
    iplSetSceneMaterial(scene, matIndex, material);
}

IPLhandle * SteamAudioInterface::CreateStaticMesh(std::vector<IPLVector3> vertices, std::vector<IPLTriangle> indices, int materialIndex) {
    IPLhandle mesh;

    // Create mesh
    iplCreateStaticMesh(scene, vertices.size(), indices.size() / 3, &mesh);
    iplSetStaticMeshVertices(scene, mesh, vertices.data());
    iplSetStaticMeshTriangles(scene, mesh, indices.data());

    IPLint32* matArray = new IPLint32[indices.size() / 3];
    for (int i = 0; i < indices.size() / 3; i++)
        matArray[i] = materialIndex;

    iplSetStaticMeshMaterials(scene, mesh, matArray);

    return nullptr;
}

void SteamAudioInterface::CreateEnvironment() {
    IPLerror err = iplCreateEnvironment(context, NULL, simSettings, scene, NULL, &environment);
    sAudio.CreateRenderers(environment);
}

void SteamAudioInterface::SetPlayer(IPLVector3 playerPos, IPLVector3 playerDir, IPLVector3 playerUp) {
    sAudio.SetPlayer(playerPos, playerDir, playerUp);
}

void SteamAudioInterface::Process(std::vector<float*>& buffers, std::vector<IPLVector3>& positions, std::vector<float>& radii, std::vector<unsigned int>& guids, float* output) {
    assert(buffers.size() == positions.size());
    assert(positions.size() == radii.size());
    assert(radii.size() == guids.size());
    std::size_t size = buffers.size();
    std::vector<SteamAudio::SoundSourceInfo> inputs;
    inputs.resize(size);
    //Create a steam audio buffer
    IPLAudioFormat format;
    format.channelLayoutType = IPL_CHANNELLAYOUTTYPE_SPEAKERS;
    format.channelLayout = IPL_CHANNELLAYOUT_MONO; //Only take mono input
    format.numSpeakers = 1;
    format.speakerDirections = NULL;
    format.channelOrder = IPL_CHANNELORDER_INTERLEAVED;
    for (std::size_t i = 0; i < size; ++i) {
        SteamAudio::SoundSourceInfo& input = inputs[i];
        IPLAudioBuffer& buf = input.buffer;
        buf.format = format;
        buf.numSamples = CHUNK_SIZE;
        buf.interleavedBuffer = buffers[i];
        buf.deinterleavedBuffer = NULL;
        input.position = positions[i];
        input.radius = radii[i];
        input.GUID = guids[i];
    }

    IPLAudioBuffer finalBuffer;
    format.channelLayout = IPL_CHANNELLAYOUT_STEREO;
    format.numSpeakers = 2;
    finalBuffer.format = format;
    finalBuffer.numSamples = CHUNK_SIZE;
    finalBuffer.interleavedBuffer = output;
    finalBuffer.deinterleavedBuffer = NULL;

    sAudio.Process(inputs, finalBuffer);
}
