#pragma once

#include <glm/glm.hpp>

namespace Video {
    class RenderTarget;
    class Filter;
    namespace Geometry {
        class Rectangle;
    }
    
    /// Applies post-processing effects to the rendered image.
    class PostProcessing {
        public:
            /// Create new post-processing handler.
            /**
             * @param screenSize The size of the screen in pixels.
             * @param rectangle %Rectangle to use for rendering.
             */
            PostProcessing(const glm::vec2& screenSize, const Geometry::Rectangle* rectangle);
            
            /// Destructor.
            ~PostProcessing();
            
            /// Get render target.
            /**
             * @return Target to render to.
             */
            RenderTarget* GetRenderTarget() const;
    
            /// Updates the buffer size.
            /**
             * @param screenSize The size of the screen in pixels.
             */
            void UpdateBufferSize(const glm::vec2& screenSize);
            
            /// Apply a filter to the render image.
            /**
             * filter %Filter to apply.
             */
            void ApplyFilter(Filter* filter);
            
            /// Render resulting image to screen.
            /**
             * @param dither Whether to use dithering.
             */
            void Render(bool dither = false);
            
        private:
            short int which = 0;
            RenderTarget* buffers[2];
            const Geometry::Rectangle* rectangle;
    };
}
