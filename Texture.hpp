#ifndef UNTITLED_TEXTURE_HPP
#define UNTITLED_TEXTURE_HPP

#include <string>
#include <iostream>

class Texture {
public:
    explicit Texture(std::string path);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unbind() const;

    [[nodiscard]] inline int getWidth() const { return width; }
    [[nodiscard]] inline int getHeight() const { return height; }
    [[nodiscard]] inline unsigned int getId() const { return id; }
    [[nodiscard]] inline int getSlot() const { return boundSlot; }
private:
    unsigned int id;
    std::string filePath;
    unsigned char* localBuffer;
    int width, height, bpp;
    mutable int boundSlot;
};

#endif //UNTITLED_TEXTURE_HPP
