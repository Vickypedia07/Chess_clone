#pragma once
#include <vector>

#include "board.h"
#include "move.h"

std::vector<Move> generatePsuedoLegal(const Board& board);