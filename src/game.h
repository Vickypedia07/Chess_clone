#pragma once
#include <memory>
#include <vector>
#include <string>
#include "board.h"
#include "move.h"
#include "rules.h"

enum class GameStatus { InProgress , Checkmate , Stalemate , Draw} ;

class Game{
public:
    Game();
    explicit Game(const Board& start);

    const Board& board() const{return board_;}
    GameStatus status() const{return status_;}
    Color turn() const{return board_.sideToMove;}
    Color winner() const;
    bool inCheck() const;

    std::string statusMessage() const;
    std::vector<Move> legalMoves() const;
    const std::vector<Move>& history() const{return history_;}

    bool tryMove(int from , int to , PieceType promotion = PieceType::Queen);
private:
    void updateStatus();
    Board board_;
    std::unique_ptr<ChessRules> rules_;
    GameStatus status_ = GameStatus::InProgress;
    std::string drawReason_;
    std::vector<Move> history_;
    std::vector<std::string> positions_;
};