#pragma once

#include <glm/glm.hpp>

namespace Video {
    class ShaderProgram;
    class RenderSurface;
    class Filter;
    namespace Geometry {
        class Rectangle;
    }
    
    /// Applies post-processing effects to the rendered image.
    class PostProcessing {
        public:
            /// Create new post-processing handler.
            /**
             * @param rectangle %Rectangle to use for rendering.
             */
            PostProcessing(const Geometry::Rectangle* rectangle);
            
            /// Destructor.
            ~PostProcessing();
            
            /// Apply a filter to the render image.
            /**
             * @param renderSurface %RenderSurface to apply filter to.
             * @param filter %Filter to apply.
             */
            void ApplyFilter(Video::RenderSurface* renderSurface, Filter* filter) const;
            
            /// Render %RenderSurface to screen.
            /**
             * @param renderSurface %RenderSurface to render to screen.
             * @param dither Whether to use dithering.
             */
            void Render(Video::RenderSurface* renderSurface, bool dither = false);
            
        private:
            const Geometry::Rectangle* rectangle;

            float ditherTime = 0.f;

            ShaderProgram* shaderProgram;
            ShaderProgram* ditherShaderProgram;
    };
}
