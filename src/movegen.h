#pragma once
#include <vector>
#include <string>

#include "board.h"
#include "move.h"

std::vector<Move> generatePseudoLegal(const Board& board);
std::vector<Move> generatePseudoLegal(const Board& board, bool withCastling = true);

Board applyMove(const Board& b, Move m);
bool inCheck(const Board& b, Color c);

std::string squareName(int sq);
std::string moveName(const Move& m);


std::vector<Move> generateLegal(const Board& b);
long long perft(const Board& b , int depth);