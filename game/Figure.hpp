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

typedef bool Player;
#define WHITE false
#define BLACK true

class Figure {
public:
	Figure(FigureType type, uint8_t x, uint8_t y, Player player);

	void move(int dx, int dy);
	bool contains(float x, float y) const;

	FigureType type;
	std::array<int, 2> position;

	Player player;
};