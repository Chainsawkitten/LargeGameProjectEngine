#include "VorbisFile.hpp"

#include <algorithm>
#include <stb_vorbis.c>
#include <Utility/Log.hpp>

#ifdef USINGMEMTRACK
#include <MemTrackInclude.hpp>
#endif

using namespace Audio;

VorbisFile::VorbisFile() {

}

VorbisFile::~VorbisFile() {
    if (stbFile)
        stb_vorbis_close(stbFile);

    if (buffer)
        delete[] buffer;
}

int VorbisFile::GetData(uint32_t offset, uint32_t samples, float* data) const {
    if (!IsLoaded())
        return 0;

    assert(offset < sampleCount);

    if (buffer) {
        samples = std::min(std::max(sampleCount - (int)(offset + samples), 0), (int)samples);
        memcpy(data, &buffer[offset], sizeof(float) * samples);
        return samples;
    } else {
        stb_vorbis_seek(stbFile, offset);
        return stb_vorbis_get_samples_float_interleaved(stbFile, channelCount, data, samples);
    }
}

uint32_t VorbisFile::GetSampleCount() const {
    return sampleCount;
}

uint32_t VorbisFile::GetSampleRate() const {
    return sampleRate;
}

uint32_t VorbisFile::GetChannelCount() const {
    return channelCount;
}

void VorbisFile::Cache(bool cache) {
    if (!IsLoaded()) {
        Log() << "No OGG Vorbis file loaded to cache.\n";
        return;
    }
    
    if (cache == IsCached())
        return;

    if (buffer) {
        delete[] buffer;
        buffer = nullptr;
    }

    if (cache) {
        buffer = new float[sampleCount];
        stb_vorbis_get_samples_float_interleaved(stbFile, channelCount, buffer, sampleCount);
    }
}

void VorbisFile::Load(const char* filename) {
    // Open OGG file
    int error;
    stbFile = stb_vorbis_open_filename(filename, &error, NULL);
    if (!stbFile) {
        Log() << "Couldn't load OGG Vorbis file: " << filename << "\n";
        return;
    }

    stb_vorbis_info stbInfo = stb_vorbis_get_info(stbFile);
    channelCount = stbInfo.channels;
    sampleRate = stbInfo.sample_rate;
    sampleCount = stb_vorbis_stream_length_in_samples(stbFile) * stbInfo.channels;
}

bool VorbisFile::IsLoaded() const {
    return (stbFile != nullptr);
}
