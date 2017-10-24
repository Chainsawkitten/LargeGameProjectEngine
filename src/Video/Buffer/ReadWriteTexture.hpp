#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../linking.hpp"

namespace Video {
    /// Texture to write and read from.
    class ReadWriteTexture {
        public:
        /// Create new read/write texture.
        /**
             * @param size Size of the new read/write texture.
             * @param format GL format for the texture, numner of channels.
             * @param internalFormat GL internal format, bits per channel.
             * @param type Type for pixel data.
             */
        VIDEO_API ReadWriteTexture(const glm::vec2& size, GLenum format, GLint internalFormat, GLenum type);

        /// Destructor.
        VIDEO_API ~ReadWriteTexture();

        /// Get size of texture.
        /**
             * @return The size of texture in pixels.
             */
        VIDEO_API glm::vec2 GetSize() const;

        /// Get texture.
        /**
             * @return The texture.
             */
        VIDEO_API GLuint GetTexture() const;

        /// Get format.
        /**
             * @return Format of texture.
             */
        VIDEO_API GLenum GetFormat() const;

        /// Bind texture for reading.
        /**
             * @param binding Slot to bind texture to.
             */
        VIDEO_API void BindForReading(GLenum binding) const;

        private:
        static void CreateTexture(GLuint& texture, unsigned width, unsigned height, GLenum format, GLint internalFormat, GLenum type);

        GLuint texture;
        GLenum format;

        glm::vec2 size;
    };
}
