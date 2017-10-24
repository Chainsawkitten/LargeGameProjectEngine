#include "StorageBuffer.hpp"

#include <assert.h>
#include <cstring>

#include <Utility/Log.hpp>
#include <ErrorVideoDebug.hpp>

using namespace Video;

StorageBuffer::StorageBuffer(unsigned int size, GLenum usage)
    : bound(false) {
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)size, NULL, usage);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    this->size = size;
}

StorageBuffer::~StorageBuffer() {
    glDeleteBuffers(1, &ssbo);
}

void StorageBuffer::Write(void* data, unsigned int offset, unsigned int length) {
    assert(bound);
    assert(this->size >= offset + length);
    ERROR_CHECK_VIDEO_START
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, length, data);
    ERROR_CHECK_VIDEO_END
}

void StorageBuffer::Bind() {
    if (bound) {
        Log() << "StorageBuffer::Bind Warning: Already bound.\n";
        return;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    bound = true;
}

void StorageBuffer::Unbind() {
    if (!bound) {
        Log() << "StorageBuffer::Unbind Warning: Not bound.\n";
        return;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    bound = false;
}

void StorageBuffer::BindBase(unsigned int binding) const {
    ERROR_CHECK_VIDEO_START
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLuint)binding, ssbo);
    ERROR_CHECK_VIDEO_END
}

unsigned int StorageBuffer::GetSize() const {
    return size;
}
