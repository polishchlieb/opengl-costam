#ifndef UNTITLED_VERTEXBUFFERLAYOUT_HPP
#define UNTITLED_VERTEXBUFFERLAYOUT_HPP

#include <glad/glad.h>
#include <vector>
#include <iostream>

struct Element {
    unsigned int count, type, normalized;

    static unsigned int getSizeOfType(unsigned int type) {
        switch (type) {
            case GL_FLOAT: return sizeof(float);
            case GL_UNSIGNED_INT: return sizeof(unsigned int);
            case GL_INT: return sizeof(int);
            default: break;
        }
        return 0;
    }
};

class VertexBufferLayout {
public:
    void push(unsigned int type, unsigned int count, unsigned int normalized = GL_FALSE) {
        elements.push_back({ .count = count, .type = type, .normalized = normalized });
        stride += count * Element::getSizeOfType(type);
    }

    [[nodiscard]] unsigned int getStride() const {
        return stride;
    }

    [[nodiscard]] const std::vector<Element>& getElements() const {
        return elements;
    }
private:
    std::vector<Element> elements;
    unsigned int stride = 0;
};

#endif //UNTITLED_VERTEXBUFFERLAYOUT_HPP
