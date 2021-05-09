#pragma once
#include <string>

class Image {
public:
	Image(const std::string& filePath);
	~Image();

	size_t getWidth() const { return width; }
	size_t getHeight() const { return height; }
	const unsigned char* getBuffer() const { return buffer; }
private:
	size_t width, height;
	unsigned char* buffer;
};