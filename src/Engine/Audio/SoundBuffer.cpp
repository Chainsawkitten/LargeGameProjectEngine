#include "SoundBuffer.hpp"

#include <algorithm>
#include "../Manager/SoundManager.hpp"
#include "SoundFile.hpp"
#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"
#include "VorbisFile.hpp"
#include <Utility/Log.hpp>
#include "../Audio/SoundStreamer.hpp"
#include "../Manager/Managers.hpp"
#include <cstring>

#ifdef USINGMEMTRACK
#include <MemTrackInclude.hpp>
#endif

using namespace Audio;

SoundBuffer::SoundBuffer() {

}

SoundBuffer::~SoundBuffer() {
    SetSoundFile(nullptr);
}

float* SoundBuffer::GetChunkData(int& samples) {
    if (!soundFile) {
        Log() << "SoundBuffer::GetBuffer: No sound loaded.\n";
        samples = 0;
        return nullptr;
    }

    assert(!chunkQueue.empty());

    SoundStreamer::DataHandle& handle = chunkQueue.front();
    while (!handle.done);

    if (handle.samples < CHUNK_SIZE)
        memset(&handle.data[(handle.offset + handle.samples) % (CHUNK_SIZE * CHUNK_COUNT)], 0, sizeof(CHUNK_SIZE - handle.samples));

    samples = handle.samples;
    return handle.data;
}

void SoundBuffer::ConsumeChunk() {
    assert(soundFile);
    chunkQueue.pop();
}

void SoundBuffer::ProduceChunk() {
    assert(soundFile);
    chunkQueue.push(Audio::SoundStreamer::DataHandle(soundFile, begin, CHUNK_SIZE, &buffer[begin % (CHUNK_SIZE * CHUNK_COUNT)]));
    Managers().soundManager->Load(chunkQueue.back());
    begin += CHUNK_SIZE;
}

SoundFile* SoundBuffer::GetSoundFile() const {
    return soundFile;
}

void SoundBuffer::SetSoundFile(SoundFile* soundFile) {

    // Remove old sound file.
    if (this->soundFile) {
        begin = 0;
        Managers().soundManager->Flush(chunkQueue);
    }

    // Update sound file.
    this->soundFile = soundFile;
    assert(chunkQueue.empty());

    // Set new sound file.
    if (soundFile)
        for (int i = 0; i < CHUNK_COUNT; ++i)
            ProduceChunk();

}

void SoundBuffer::Restart() {
    assert(soundFile);
    begin = 0;
    Managers().soundManager->Flush(chunkQueue);
    for (int i = 0; i < CHUNK_COUNT; ++i)
        ProduceChunk();
}
