#pragma once

#include "SuperComponent.hpp"

namespace Video {
    namespace Geometry {
        class Geometry3D;
    }
}

namespace Component {
    /// %Component providing geometry to an entity.
    class Mesh : public SuperComponent {
        public:
            /// Create new mesh.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            Mesh(Entity* entity);
            
            /// Destructor.
            ~Mesh() override;
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;
            
            /// Load component from JSON node.
            /**
             * @param node JSON node to load from.
             */
            void Load(const Json::Value& node) override;

            /// Geometry data.
            /**
             * Default: nullptr
             */
            Video::Geometry::Geometry3D* geometry = nullptr;

    };
}
