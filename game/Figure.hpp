#pragma once
#include <array>
#include <cstdint>

enum class FigureType {
	rook,
	knight,
	bishop,
	pawn,
	king,
	queen
};

class Figure {
public:
	Figure(FigureType type, uint8_t x, uint8_t y);

	void move(int dx, int dy);
	bool contains(float x, float y) const;

	FigureType type;
	std::array<int, 2> position;
};