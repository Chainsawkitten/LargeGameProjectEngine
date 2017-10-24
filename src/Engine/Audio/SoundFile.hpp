#pragma once

#include <cstdint>
#include <AL/al.h>

namespace Audio {
    /// Interface for sound files of various formats.
    /**
     * Used to get raw audio data to a SoundBuffer.
     */
    class SoundFile {
        public:
        /// Destructor.
        virtual ~SoundFile() {}

        /// Get raw audio data.
        /**
             * @return Raw audio data.
             */
        virtual float* GetData() const = 0;

        /// Get data size.
        /**
             * @return The length of the raw audio data.
             */
        virtual uint32_t GetSize() const = 0;

        /// Get sample rate.
        /**
             * @return The sound file's sample rate (Hz).
             */
        virtual uint32_t GetSampleRate() const = 0;
    };
}
