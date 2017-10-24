#pragma once

#include <memory>
#include "SuperComponent.hpp"
#include "../linking.hpp"

class PhysicsManager;

namespace Physics {
    class Shape;
}

namespace Component {
    /// Physical shape that can be used in conjunction with rigid bodies or to
    /// act as a trigger volume.
    class Shape : public SuperComponent {
        friend class ::PhysicsManager;

        public:
        /// Create new shape.
        ENGINE_API Shape() = default;

        /// Save the component.
        /**
             * @return JSON value to be stored on disk.
             */
        ENGINE_API Json::Value Save() const override;

        /// Get the shape this component represents.
        /**
             * @return The %Shape.
             */
        ENGINE_API std::shared_ptr<::Physics::Shape> GetShape() const;

        private:
        // Set the shape that this component will represent.
        void SetShape(std::shared_ptr<::Physics::Shape> shape);

        std::shared_ptr<::Physics::Shape> shape = nullptr;
    };
}
