#include "Chessboard.hpp"

Chessboard::Chessboard() {
	figures.reserve(32);

	figures.emplace_back(FigureType::rook, 0, 0, WHITE);
	figures.emplace_back(FigureType::knight, 1, 0, WHITE);
	figures.emplace_back(FigureType::bishop, 2, 0, WHITE);
	figures.emplace_back(FigureType::queen, 3, 0, WHITE);
	figures.emplace_back(FigureType::king, 4, 0, WHITE);
	figures.emplace_back(FigureType::bishop, 5, 0, WHITE);
	figures.emplace_back(FigureType::knight, 6, 0, WHITE);
	figures.emplace_back(FigureType::rook, 7, 0, WHITE);

	figures.emplace_back(FigureType::rook, 0, 7, BLACK);
	figures.emplace_back(FigureType::knight, 1, 7, BLACK);
	figures.emplace_back(FigureType::bishop, 2, 7, BLACK);
	figures.emplace_back(FigureType::queen, 3, 7, BLACK);
	figures.emplace_back(FigureType::king, 4, 7, BLACK);
	figures.emplace_back(FigureType::bishop, 5, 7, BLACK);
	figures.emplace_back(FigureType::knight, 6, 7, BLACK);
	figures.emplace_back(FigureType::rook, 7, 7, BLACK);

	for (uint8_t x = 0; x < 8; ++x)
		figures.emplace_back(FigureType::pawn, x, 1, WHITE);
	for (uint8_t x = 0; x < 8; ++x)
		figures.emplace_back(FigureType::pawn, x, 6, BLACK);
}
