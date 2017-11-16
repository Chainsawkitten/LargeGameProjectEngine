#pragma once

#include "Texture2D.hpp"
#include <cstdint>

namespace Video {
    /// Texture loaded from custom compressed texture format.
    /**
     * @todo DXT1 compression.
     * @todo Mipmaps.
     */
    class TextureHCT : public Texture2D {
        public:
        /// Load texture.
        /**
         * @param filenameThe name of the HCT file to load.
         */
        VIDEO_API TextureHCT(const char* filename);
        
        /// Destructor.
        VIDEO_API ~TextureHCT() override;
        
        /// Get OpenGL texture ID.
        /**
         * Used when binding a texture before draw calls.
         * @return The OpenGL texture identifier
         */
        VIDEO_API GLuint GetTextureID() const override;
        
        /// Get whether the texture has been loaded yet.
        /**
         * @return Whether the texture has been loaded yet.
         */
        VIDEO_API bool IsLoaded() const override;
        
        /// The version of the texture format.
        static const uint16_t VERSION = 3;
        
        private:
        GLuint texID = 0;
        bool loaded = false;
    };
}
