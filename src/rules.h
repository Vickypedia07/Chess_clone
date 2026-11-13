#pragma once
#include <vector>
#include "board.h"
#include "move.h"

class ChessRules{
public:
    virtual ~ChessRules() = default;
    virtual std::vector<Move> legalMoves(const Board& b) const = 0;
    virtual bool isInCheck(const Board& b, Color c ) const = 0;
    virtual bool isCheckmate(const Board& b) const = 0;
    virtual bool isStalemate(const Board& b) const = 0;
    virtual bool isInsufficientMaterial(const Board& b) const = 0;
};

class StandardChessRules : public ChessRules{
public:
    std::vector<Move> legalMoves(const Board& b) const override;
    bool isInCheck(const Board& b, Color c ) const override;
    bool isCheckmate(const Board& b) const override;
    bool isStalemate(const Board& b) const override;
    bool isInsufficientMaterial(const Board& b) const override;
};