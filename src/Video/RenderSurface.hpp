#pragma once

#include <glm/glm.hpp>
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
            RenderSurface(const glm::vec2& size);
            
            /// Destructor.
            ~RenderSurface();
            
            /// Get size of the render target.
            /**
            * @return The size of the render target in pixels.
            */
            glm::vec2 GetSize() const;

            /// Get deferred rendering frame buffer.
            /**
             * @return Deferred rendering frame buffer.
             */
            FrameBuffer* GetDeferredFrameBuffer() const;

            /// Get post processing frame buffer.
            /**
            * @return Post processing frame buffer.
            */
            FrameBuffer* GetPostProcessingFrameBuffer() const;

            /// Get texture contaning albedo pixel data.
            /**
             * @return The albedo texture.
             */
            ReadWriteTexture* GetAlbedoTexture() const;

            /// Get texture contaning normal pixel data.
            /**
             * @return The normal texture.
             */
            ReadWriteTexture* GetNormalTexture() const;

            /// Get texture contaning specular pixel data.
            /**
             * @return The specular texture.
             */
            ReadWriteTexture* GetSpecularTexture() const;

            /// Get texture contaning glow pixel data.
            /**
             * @return The glow texture.
             */
            ReadWriteTexture* GetGlowTexture() const;

            /// Get texture contaning depth pixel data.
            /**
             * @return The depth texture.
             */
            ReadWriteTexture* GetDepthTexture() const;

            /// Get texture contaning color pixel data.
            /**
             * @return The color texture.
             */
            ReadWriteTexture* GetColorTexture() const;

            /// Get texture contaning extra color pixel data.
            /**
             * @return The extra color texture.
             */
            ReadWriteTexture* GetExtraColorTexture() const;

            /// Get texture contaning extra depth pixel data.
            /**
             * @return The extra depth texture.
             */
            ReadWriteTexture* GetExtraDepthTexture() const;

            /// Swap active texture set.
            void Swap();
            
        private:
            RenderSurface(const RenderSurface & other) = delete;
            glm::vec2 size;

            // Frame buffers.
            FrameBuffer* deferredFrameBuffer;
            FrameBuffer* postProcessingFrameBuffer[2];

            // Textures.
            ReadWriteTexture* albedoTexture;
            ReadWriteTexture* normalTexture;
            ReadWriteTexture* specTexture;
            ReadWriteTexture* glowTexture;
            ReadWriteTexture* depthTexture;
            ReadWriteTexture* colorTexture[2];
            ReadWriteTexture* extraColorTexture[2];
            ReadWriteTexture* extraDepthTexture[2];

            short int which = 0;
    };
}
