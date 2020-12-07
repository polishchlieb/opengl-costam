#ifndef UNTITLED_BUFFER_HPP
#define UNTITLED_BUFFER_HPP

#include <glad/glad.h>

class Buffer {
public:
    Buffer();
    Buffer(unsigned int type, const void* data, unsigned int size, unsigned int usage);
    ~Buffer();

    void bind() const;
    void unbind() const;

    void setData(const void* data, unsigned int size);

    [[nodiscard]] unsigned int getSize();
    [[nodiscard]] unsigned int getId() const {
        return id;
    }
private:
    unsigned int id{}, type{}, size{}, usage{};
};

#endif //UNTITLED_BUFFER_HPP
