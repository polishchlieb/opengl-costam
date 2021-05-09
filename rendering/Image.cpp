#include "Image.hpp"
#include <stb_image.h>

Image::Image(const std::string& filePath) {
	int width, height, bpp;
	stbi_set_flip_vertically_on_load(true);
	buffer = stbi_load(filePath.c_str(), &width, &height, &bpp, 4);

	this->width = static_cast<size_t>(width);
	this->height = static_cast<size_t>(height);
}

Image::~Image() {
	stbi_image_free(buffer);
}
