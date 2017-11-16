#pragma once

/// Functions for texture conversion.
namespace TextureConverter {
    /// Convert texture from PNG to HCT format.
    /**
     * @param inFilename Filename of PNG file.
     * @param outFilename Filename of HCT file.
     */
    void Convert(const char* inFilename, const char* outFilename);
}
