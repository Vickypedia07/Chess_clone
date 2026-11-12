#pragma once

#include <array>
#include <vector>
#include <string>
#include "piece.h"

struct Board {
    std::array<Piece,64> squares{};
    Color sideToMove = Color::White;
    bool castleWK=true,castleWQ=true,castleBK=true,castleBQ=true;
    int enPassant = -1;

    Piece at(int row, int col) const {
        return squares[row*8+col];
    }

    void set(int row, int col, Piece p){
        squares[row*8+col] = p;
    }

    static Board startingPosition();

    static Board fromRows(const std::vector<std::string>& rows, Color side);

    static Board fromFen(const std::string& fen);
};
