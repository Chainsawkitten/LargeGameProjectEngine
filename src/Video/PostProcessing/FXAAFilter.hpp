#pragma once

#include "Filter.hpp"
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace Video {
    /// Fast approximate anti-aliasing filter.
    class FXAAFilter : public Filter {
        public:
        /// Create new FXAA filter.
        FXAAFilter();

        /// Free allocated resources.
        ~FXAAFilter();

        /// Get shader program.
        /**
             * @return Shader program
             */
        Video::ShaderProgram* GetShaderProgram() const;

        /// Set uniforms.
        void SetUniforms();

        /// Set the screen size used when calculating FXAA.
        /**
             * @param screenSize Size of the screen in pixels.
             */
        void SetScreenSize(const glm::vec2& screenSize);

        private:
        ShaderProgram* shaderProgram;

        glm::vec2 screenSize;
        GLint screenSizeLocation;
    };
}
