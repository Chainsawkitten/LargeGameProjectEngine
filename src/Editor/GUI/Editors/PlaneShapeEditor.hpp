#pragma once

#include "IShapeEditor.hpp"

namespace Component {
    class Shape;
}

namespace GUI {

    /// Plane shape editor for physics components.
    class PlaneShapeEditor : public IShapeEditor {
        public:
        /// Constructor
        PlaneShapeEditor() = default;

        /// Plane type label.
        /**
             * @return "Plane" string.
             */
        virtual const char* Label() const override { return "Plane"; }

        /// Show controls for editing a plane shape.
        /**
             * @param comp The shape component to edit.
             */
        virtual void Show(Component::Shape* comp) override;

        /// Set a sphere shape on the given shape component.
        /**
             * @param comp The shape component on which to set shape.
             */
        virtual void Apply(Component::Shape* comp) override;

        /// Set internal data according the given %Shape, provided that it
        /// is a plane.
        /**
             * @param shape The %Shape from which to initialize data.
             * @return True if shape type is plane, false otherwise.
             */
        virtual bool SetFromShape(const Physics::Shape& shape) override;

        private:
        float normal[3] = { 0.0f, 1.0f, 0.0f };
        float planeCoeff = 0.0f;
    };
}
