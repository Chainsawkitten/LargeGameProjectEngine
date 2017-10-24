#pragma once

#include <glm/glm.hpp>
#include "linking.hpp"

namespace Video {

    class FrameBuffer;
    class ReadWriteTexture;

    /// Render surface containing framebuffers.
    class RenderSurface {
        public:
        /// Create new render surface.
        /**
             * @param size Size of the new render surface.
             */
        VIDEO_API RenderSurface(const glm::vec2& size);

        /// Destructor.
        VIDEO_API ~RenderSurface();

        /// Get size of the render target.
        /**
            * @return The size of the render target in pixels.
            */
        VIDEO_API glm::vec2 GetSize() const;

        /// Get depth frame buffer.
        /**
             * @return Depth frame buffer.
             */
        VIDEO_API FrameBuffer* GetDepthFrameBuffer() const;

        /// Get shading frame buffer.
        /**
             * @return Shading frame buffer.
             */
        VIDEO_API FrameBuffer* GetShadingFrameBuffer() const;

        /// Get color frame buffer.
        /**
             * @return Color rendering frame buffer.
             */
        VIDEO_API FrameBuffer* GetColorFrameBuffer() const;

        /// Get texture contaning depth pixel data.
        /**
             * @return The depth texture.
             */
        VIDEO_API ReadWriteTexture* GetDepthTexture() const;

        /// Get texture contaning color pixel data.
        /**
             * @return The color texture.
             */
        VIDEO_API ReadWriteTexture* GetColorTexture() const;

        /// Swap active texture set.
        VIDEO_API void Swap();

        /// Clear all textures in RenderSurface.
        VIDEO_API void Clear() const;

        private:
        RenderSurface(const RenderSurface& other) = delete;
        glm::vec2 size;

        // Frame buffers.
        FrameBuffer* depthFrameBuffer;
        FrameBuffer* colorFrameBuffer[2];
        FrameBuffer* shadingFrameBuffer[2];

        // Textures.
        ReadWriteTexture* depthTexture;
        ReadWriteTexture* colorTexture[2];

        short int which = 0;
    };
}
