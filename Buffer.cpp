#include "Buffer.hpp"

Buffer::Buffer(
    unsigned int type,
    const void *data,
    unsigned int size,
    unsigned int usage
)
    : type(type), size(size), usage(usage)
{
    glGenBuffers(1, &id);
    glBindBuffer(type, id);
    glBufferData(type, size, data, usage);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &id);
}

void Buffer::bind() const {
    glBindBuffer(type, id);
}

void Buffer::unbind() const {
    glBindBuffer(type, 0);
}

unsigned int Buffer::getSize() {
    return size;
}

void Buffer::setData(const void* data, unsigned int size) {
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

Buffer::Buffer() = default;