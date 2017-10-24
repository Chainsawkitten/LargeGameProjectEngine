#include "VorbisFile.hpp"

#include <Utility/Log.hpp>
#include <stb_vorbis.c>

using namespace Audio;

VorbisFile::VorbisFile(const char* filename) {
    // Open OGG file
    int error;
    stb_vorbis* stbfile = stb_vorbis_open_filename(filename, &error, NULL);
    if (error == VORBIS_file_open_failure)
        Log() << "Couldn't load OGG Vorbis file: " << filename << "\n";

    stb_vorbis_info info = stb_vorbis_get_info(stbfile);
    const int samples = stb_vorbis_stream_length_in_samples(stbfile) * info.channels;
    sampleRate = info.sample_rate;

    // Get data
    data = new float[samples * sizeof(float)];
    stb_vorbis_get_samples_float_interleaved(stbfile, info.channels, data, samples);

    dataSize = samples;
}

VorbisFile::~VorbisFile() {
}

float* VorbisFile::GetData() const {
    return data;
}

uint32_t VorbisFile::GetSize() const {
    return dataSize;
}

uint32_t VorbisFile::GetSampleRate() const {
    return sampleRate;
}
