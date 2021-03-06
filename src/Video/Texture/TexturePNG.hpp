#pragma once

#include "Texture2D.hpp"

namespace Video {
    /// Texture loaded from a PNG file.
    class TexturePNG : public Texture2D {
        public:
        /// Create new PNG texture.
        /**
         * @param source Source string containing the image file.
         * @param sourceLength Length of the source string.
         */
        VIDEO_API TexturePNG(const char* source, int sourceLength);
        
        /// Destructor.
        VIDEO_API ~TexturePNG() override;
        
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
        
        private:
        // Get image GL format based on color components.
        static GLenum Format(int components);
        
        GLuint texID = 0;
        bool loaded = false;
    };
}
