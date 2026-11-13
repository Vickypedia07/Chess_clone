#include "game.h"
#include "movegen.h"

#include <algorithm>

namespace{

std::string positionKey(const Board& b){
    std::string key;
    for(const Piece& p : b.squares){
        if(p.empty()) {key += '.'; continue;}
        key += static_cast<char>('0' + static_cast<int>(p.type));
        key += (p.color==Color::White) ? 'w' : 'b';
    }
    key += (b.sideToMove==Color::White) ? 'W' : 'B';
    key += b.castleWK ? 'K' : '-';
    key += b.castleWQ ? 'Q' : '-';
    key += b.castleBK ? 'k' : '-';
    key += b.castleBQ ? 'q' : '-';
    key += std::to_string(b.enPassant);

    return key;
}
}

Game::Game() : Game(Board::startingPosition()){}

Game::Game(const Board& start) : board_(start), rules_(std::make_unique<StandardChessRules>()){
    positions_.push_back(positionKey(board_));
    updateStatus();
}

Color Game::winner() const{return opposite(board_.sideToMove);}

bool Game::inCheck() const{return rules_->isInCheck(board_,board_.sideToMove);}

std::vector<Move> Game::legalMoves() const{
    if(status_ != GameStatus::InProgress) return{};
    return rules_->legalMoves(board_);
}

bool Game::tryMove(int from, int to, PieceType promotion){
    if(status_ != GameStatus::InProgress) return false;
    for( const Move& m : rules_->legalMoves(board_)){
        if(m.from != from || m.to != to) continue;
        if(m.promotion != PieceType::None && m.promotion != promotion) continue;
        board_ = applyMove(board_,m);
        history_.push_back(m);
        positions_.push_back(positionKey(board_));
        updateStatus();
        return true;
    }
    return false;
}

void Game::updateStatus(){
    if (rules_->isCheckmate(board_)) { status_ = GameStatus::Checkmate; return; }
    if (rules_->isStalemate(board_)) { status_ = GameStatus::Stalemate; return; }
    if (rules_->isInsufficientMaterial(board_)) {
        status_ = GameStatus::Draw;
        drawReason_ = "insufficient material";
        return;
    }
    if (board_.halfmoveClock >= 100) {
        status_ = GameStatus::Draw;
        drawReason_ = "the 50-move rule";
        return;
    }
    const auto seen = std::count(positions_.begin(), positions_.end(), positions_.back());
    if (seen >= 3) {
        status_ = GameStatus::Draw;
        drawReason_ = "threefold repetition";
    }
}

std::string Game::statusMessage() const {
    switch (status_) {
        case GameStatus::Checkmate:
            return std::string("Checkmate! ") + (winner() == Color::White ? "White" : "Black") +
                   " wins. Press R to restart";
        case GameStatus::Stalemate:
            return "Stalemate: draw. Press R to restart";
        case GameStatus::Draw:
            return "Draw by " + drawReason_ + ". Press R to restart";
        default: {
            const std::string side = (turn() == Color::White) ? "White" : "Black";
            return inCheck() ? side + " is in check" : side + " to move";
        }
    }
}
