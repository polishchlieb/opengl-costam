#pragma once
#include "Figure.hpp"
#include <vector>

class Chessboard {
public:
	Chessboard();
	std::vector<Figure> figures;
};