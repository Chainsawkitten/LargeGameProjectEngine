#pragma once

#include "SuperComponent.hpp"
#include "../Geometry/Model.hpp"
#include "../linking.hpp"

namespace Geometry {
    class Model;
}

namespace Component {
    /// %Component handling animations.
    class Animation : public SuperComponent {
        public:
        /// Create new %Animation component.
        ENGINE_API Animation();

        /// Save the component.
        /**
             * @return JSON value to be stored on disk.
             */
        ENGINE_API Json::Value Save() const override;

        /// Rigged model to animate.
        /**
             * Default: nullptr
             */
        Geometry::Model* riggedModel = nullptr;

        /// Total time of animation.
        /**
             * Default: 0
             */
        float time = 0.f;
    };
}
