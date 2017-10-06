#pragma once

#include <string>
#include <json/json.h>

namespace Video {
    class Texture2D;
}

/// A texture used in a hymn.
class TextureAsset {
    public:
        /// Create new texture asset.
        TextureAsset();

        /// Destructor.
        ~TextureAsset();
        
        /// Save the texture asset.
        void Save() const;
        
        /// Load texture asset from disk.
        /**
         * @param name The name of the texture asset.
         */
        void Load(const std::string& name);

        /// Load texture asset from memory.
        /**
         * @param name The name of the texture asset.
         * @param texture The texture we will load our texture information from.
         * @param srgb Whether to convert from SRGB space to linear space when loading.
         */
        void Load(const std::string& name, Video::Texture2D* texture, bool srgb = false);
        
        /// Get the texture.
        /**
         * @return The texture.
         */
        Video::Texture2D* GetTexture() const;
        
        /// The name of the texture.
        std::string name;
        
        /// Whether to convert from SRGB space to linear space when loading.
        bool srgb = false;
        
    private:
        TextureAsset(const TextureAsset & other) = delete;
        Video::Texture2D* texture;
};
