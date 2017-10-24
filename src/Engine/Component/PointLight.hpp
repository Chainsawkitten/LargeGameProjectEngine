#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>
#include "../linking.hpp"

namespace Component {
    /// %Component describing a point light.
    class PointLight : public SuperComponent {
        public:
        /// Create new point light.
        ENGINE_API PointLight();

        /// Save the component.
        /**
             * @return JSON value to be stored on disk.
             */
        ENGINE_API Json::Value Save() const override;

        /// Color.
        glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);

        /// Attenuation.
        float attenuation = 1.f;

        /// Intensity.
        float intensity = 1.f;
    };
}
