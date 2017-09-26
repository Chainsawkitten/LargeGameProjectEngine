#pragma once

#include "SuperComponent.hpp"

class Entity;
class TextureAsset;

namespace Component {
    /// %Component providing a material to Entity.
    class Material : public SuperComponent {
        public:
            /// Create new material.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            Material(Entity* entity);
            
            /// Destructor.
            ~Material() override;
            
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
            
            /// Albedo texture.
            TextureAsset* albedo;
            
            /// Normal texture.
            TextureAsset* normal;
            
            /// Metallic texture.
            TextureAsset* metallic;
            
            /// Roughness texture.
            TextureAsset* roughness;
            
        private:
            void LoadTexture(TextureAsset*& texture, const std::string& name);
    };
}
