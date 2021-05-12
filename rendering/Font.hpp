#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Texture.hpp"
#include <glm/glm.hpp>

struct Character {
	std::unique_ptr<Texture> texture;
	glm::ivec2 bearing;
	unsigned int advance;
};

class Font {
public:
	Font(const std::string& filePath, size_t size);

	const Character& getChar(char c) const { return characters.find(c)->second; }
	float measureText(const std::string& text, float scale = 1.f) const;
private:
	std::unordered_map<char, Character> characters;
};
