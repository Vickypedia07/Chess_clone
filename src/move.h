#pragma once
#include "piece.h"

struct Move{
    int from = 0;
    int to = 0;
    PieceType promotion = PieceType::None;

};  