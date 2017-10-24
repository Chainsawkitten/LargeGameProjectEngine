#pragma once

#include <string>
#include <json/json.h>
#include <cstdint>
#include "../linking.hpp"

namespace Audio {
    class SoundFile;

    /// Wrapper for OpenAL buffers.
    class SoundBuffer {
        public:
        /// Create new unloaded sound buffer.
        ENGINE_API SoundBuffer();

        /// Create a sound buffer from a sound file.
        /**
             * @param soundFile The sound file containing the sound.
             */
        ENGINE_API SoundBuffer(SoundFile* soundFile);

        /// Destructor.
        ENGINE_API ~SoundBuffer();

        /// Get AL buffer.
        /**
             * @return The OpenAL buffer ID.
             */
        ENGINE_API float* GetBuffer() const;

        /// Save the sound.
        /**
             * @return JSON value to be stored on disk.
             */
        ENGINE_API Json::Value Save() const;

        /// Load sound from file.
        /**
             * @param name Name of the sound to load.
             */
        ENGINE_API void Load(const std::string& name);

        /// Load sound buffer from a sound file.
        /**
             * @param soundFile The sound file containing the sound.
             */
        ENGINE_API void Load(SoundFile* soundFile);

        /// Get size of buffer.
        /**
             * @return The size of the buffer.
             */
        ENGINE_API uint32_t GetSize();

        /// The name of the sound.
        std::string name;

        /// The folder containing the sound file.
        std::string path;

        private:
        float* buffer;
        uint32_t size;
        uint32_t sampleRate;
    };
}
