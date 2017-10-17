#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>
#include "../linking.hpp"

namespace Component {
    /// %Component handling a camera lens.
    class Lens : public SuperComponent {
        public:
            /// Create new lens.
            ENGINE_EXPORT Lens();
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            ENGINE_EXPORT Json::Value Save() const override;

            /// Get the projection matrix.
            /**
             * @param screenSize Screen size in pixels.
             * @return The projection matrix.
             */
            ENGINE_EXPORT glm::mat4 GetProjection(const glm::vec2& screenSize) const;
            
            /// Field of view, in degrees.
            /**
             * Default: 45.0
             */
            float fieldOfView = 45.f;
            
            /// Near plane.
            /**
             * Default: 0.5
             */
            float zNear = 0.5f;
            
            /// Far plane.
            /**
             * Default: 100.0
             */
            float zFar = 100.f;

    };
}
