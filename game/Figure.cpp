#include "Figure.hpp"

Figure::Figure(FigureType type, uint8_t x, uint8_t y)
	: position({x, y}), type(type) {}

void Figure::move(int dx, int dy) {
	position[0] += dx;
	position[1] += dy;
}

bool Figure::contains(float x, float y) const {
	return x >= 64.0f * static_cast<float>(position[0]) && x <= 64.0f * static_cast<float>(position[0]) + 42.0f
		&& y >= 64.0f * static_cast<float>(position[1]) && y <= 64.0f * static_cast<float>(position[1]) + 42.0f;
}
