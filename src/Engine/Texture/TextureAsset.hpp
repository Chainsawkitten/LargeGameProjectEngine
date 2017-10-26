#pragma once

#include <string>
#include <json/json.h>
#include "../linking.hpp"

namespace Video {
    class Texture2D;
}

/// A texture used in a hymn.
class TextureAsset {
        public:
    /// Create new texture asset.
    ENGINE_API TextureAsset();

    /// Destructor.
    ENGINE_API ~TextureAsset();

    /// Save the texture asset.
    ENGINE_API void Save() const;

    /// Load texture asset from disk.
    /**
         * @param name The name of the texture asset.
         */
    ENGINE_API void Load(const std::string& name);

    /// Get the texture.
    /**
         * @return The texture.
         */
    ENGINE_API Video::Texture2D* GetTexture() const;

    /// The name of the texture.
    std::string name;

    /// Whether to convert from SRGB space to linear space when loading.
    bool srgb = false;

    /// The folder containing the texture files.
    std::string path;

        private:
    TextureAsset(const TextureAsset& other) = delete;
    Video::Texture2D* texture;
};
