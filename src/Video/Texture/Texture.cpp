#include "Texture.hpp"

using namespace Video;

GLenum Texture::Format(int components) {
    switch (components) {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        default:
            return GL_RGBA;
    }
}
