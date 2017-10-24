#pragma once

#include "SoundFile.hpp"
#include "../linking.hpp"

namespace Audio {
    /// Ogg Vorbis implementation of SoundFile.
    /**
     * Loads .ogg-files.
     */
    class VorbisFile : public SoundFile {
        public:
        /// Loads the specified ogg-file.
        /**
             * @param filename Filename (relative or absolute) to ogg-file.
             */
        ENGINE_API VorbisFile(const char* filename);

        /// Destructor.
        ENGINE_API ~VorbisFile() final;

        /// Get raw audio data.
        /**
             * @return Raw audio data.
             */
        ENGINE_API float* GetData() const final;

        /// Get data size.
        /**
             * @return The length of the raw audio data.
             */
        ENGINE_API uint32_t GetSize() const final;

        /// Get sample rate.
        /**
             * @return The sound file's sample rate (Hz).
             */
        ENGINE_API uint32_t GetSampleRate() const final;

        private:
        float* data;
        int dataSize;
        int sampleRate;
    };
}
