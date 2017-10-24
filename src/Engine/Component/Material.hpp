#pragma once

#include "SuperComponent.hpp"
#include "../linking.hpp"

class TextureAsset;

namespace Component {
    /// %Component providing a material to Entity.
    class Material : public SuperComponent {
        public:
        /// Create new material.
        ENGINE_API Material();

        /// Destructor.
        ENGINE_API ~Material() override;

        /// Save the component.
        /**
             * @return JSON value to be stored on disk.
             */
        ENGINE_API Json::Value Save() const override;

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
