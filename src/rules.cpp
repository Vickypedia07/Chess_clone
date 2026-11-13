#include "rules.h"
#include "movegen.h"

std::vector<Move> StandardChessRules::legalMoves(const Board& b) const { return generateLegal(b); }

bool StandardChessRules::isInCheck(const Board& b, Color c) const { return inCheck(b,c); }

bool StandardChessRules::isCheckmate(const Board& b) const{
    return inCheck(b,b.sideToMove) && generateLegal(b).empty();
}

bool StandardChessRules::isStalemate(const Board& b) const{
    return !inCheck(b,b.sideToMove) && generateLegal(b).empty();
}

bool StandardChessRules::isInsufficientMaterial(const Board& b) const {
    int others = 0;
    PieceType lone = PieceType::None;
    for( const Piece& p : b.squares){
        if(p.empty() || p.type == PieceType::King) continue;
        ++others;
        lone = p.type;
    }
    if (others == 0 ) return true;
    return others == 1 && (lone == PieceType::Bishop || lone == PieceType::Knight);
    
}

