#ifndef UNTITLED_VERTEXARRAY_HPP
#define UNTITLED_VERTEXARRAY_HPP

#include "Buffer.hpp"
#include "VertexBufferLayout.hpp"

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

    void addBuffer(const Buffer& vb, const VertexBufferLayout& layout);
private:
    unsigned int id;
};

#endif //UNTITLED_VERTEXARRAY_HPP
