#pragma once

#include <Utility/Log.hpp>
#include <GL/glew.h>

#define ERROR_CHECK_VIDEO_START               \
    {                                         \
        while (glGetError() != GL_NO_ERROR) { \
            Log() << "GL Error\n";            \
        }                                     \
    }
#define ERROR_CHECK_VIDEO_END                                                    \
    {                                                                            \
        GLenum __err(glGetError());                                              \
        while (__err != GL_NO_ERROR) {                                           \
            Log() << "GL error: " << (const char*)gluErrorString(__err) << "\n"; \
            __err = glGetError();                                                \
            assert(false);                                                       \
        }                                                                        \
    }
