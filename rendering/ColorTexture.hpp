#pragma once

#include "Texture.hpp"
#include <glm/glm.hpp>

class ColorTexture : public Texture {
public:
	ColorTexture(const glm::vec4& color);
};