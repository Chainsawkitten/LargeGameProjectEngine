#pragma once

#include <GL/glew.h>
#include <vector>
#include "../linking.hpp"

namespace Video {
    class ReadWriteTexture;

    /// Frame buffer containing frame buffer object.
    class FrameBuffer {
        public:
        /// Create new frame buffer.
        /**
             * @param textures Vector of %ReadWriteTexture to create a frame buffer object.
             */
        VIDEO_API FrameBuffer(const std::vector<ReadWriteTexture*>& textures);

        /// Destructor.
        VIDEO_API ~FrameBuffer();

        /// Bind this framebuffer as render target.
        VIDEO_API void BindWrite();

        /// Bind this framebuffer as render source.
        VIDEO_API void BindRead();

        /// Unbind this framebuffer as render target.
        VIDEO_API void Unbind();

        /// Clear this framebuffer.
        VIDEO_API void Clear() const;

        private:
        std::vector<ReadWriteTexture*> textures;

        GLuint frameBufferObject;
        GLenum target;

        bool bound;
    };
}
