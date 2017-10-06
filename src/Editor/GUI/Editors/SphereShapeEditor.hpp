#pragma once

#include "IShapeEditor.hpp"

namespace Component {
    class Shape;
}

namespace GUI {

    /// Sphere shape editor for physics components.
    class SphereShapeEditor : public IShapeEditor {
        public:
            /// Sphere type label.
            /**
             * @return "Sphere" string.
             */
            virtual const char* Label() const override { return "Sphere"; }

            /// Show controls for editing a sphere shape.
            /**
             * @param comp The shape component to edit.
             */
            virtual void Show(Component::Shape* comp) override;

            /// Set a plane shape on the given shape component.
            /**
             * @param comp The shape component on which to set shape.
             */
            virtual void Apply(Component::Shape* comp) override;

            /// Set internal data according the given %Shape, provided that it
            /// is a sphere.
            /**
             * @param shape The %Shape from which to initialize data.
             * @return True if shape type is sphere, false otherwise.
             */
            virtual bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float radius = 1.0f;
    };

}
