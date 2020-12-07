#include "Chessboard.hpp"

Chessboard::Chessboard() {
	figures.reserve(32);

	for (uint8_t i = 0; i < 2; ++i) {
		figures.emplace_back(FigureType::rook, 0, i * 7);
		figures.emplace_back(FigureType::knight, 1, i * 7);
		figures.emplace_back(FigureType::bishop, 2, i * 7);
		figures.emplace_back(FigureType::queen, 3, i * 7);
		figures.emplace_back(FigureType::king, 4, i * 7);
		figures.emplace_back(FigureType::bishop, 5, i * 7);
		figures.emplace_back(FigureType::knight, 6, i * 7);
		figures.emplace_back(FigureType::rook, 7, i * 7);
	}

	for (uint8_t y = 0; y < 2; ++y) {
		for (uint8_t x = 0; x < 8; ++x) {
			figures.emplace_back(FigureType::pawn, x, y * 5 + 1);
		}
	}
}
