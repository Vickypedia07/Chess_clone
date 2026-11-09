#pragma once
#include <vector>

#include "board.h"
#include "move.h"

std::vector<Move> generatePseudoLegal(const Board& board);

Board applyMove(const Board& b, Move m);
bool inCheck(const Board& b, Color c);
std::vector<Move> generateLegal(const Board& b);
long long perft(const Board& b , int depth);