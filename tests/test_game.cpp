#include <catch2/catch_test_macros.hpp>

#include "game.h"

namespace {
int sq(const char* name) { return (8 - (name[1] - '0')) * 8 + (name[0] - 'a'); }   // "e2" -> 52
}

TEST_CASE("a new game starts with White to move") {
    Game g;
    CHECK(g.status() == GameStatus::InProgress);
    CHECK(g.turn() == Color::White);
    CHECK(g.history().empty());
}

TEST_CASE("turns alternate and illegal moves are refused") {
    Game g;
    CHECK_FALSE(g.tryMove(sq("e2"), sq("e5")));          // a pawn cannot jump three squares
    CHECK(g.turn() == Color::White);
    CHECK(g.tryMove(sq("e2"), sq("e4")));
    CHECK(g.turn() == Color::Black);
    CHECK_FALSE(g.tryMove(sq("e2"), sq("e3")));          // the square is empty now
    CHECK(g.history().size() == 1);
}

TEST_CASE("fool's mate is checkmate") {
    Game g;
    CHECK(g.tryMove(sq("f2"), sq("f3")));
    CHECK(g.tryMove(sq("e7"), sq("e5")));
    CHECK(g.tryMove(sq("g2"), sq("g4")));
    CHECK(g.tryMove(sq("d8"), sq("h4")));
    CHECK(g.status() == GameStatus::Checkmate);
    CHECK(g.winner() == Color::Black);
    CHECK(g.legalMoves().empty());
    CHECK_FALSE(g.tryMove(sq("a2"), sq("a3")));          // no moves after the game is over
}

TEST_CASE("back rank mate") {
    Game g(Board::fromFen("6k1/5ppp/8/8/8/8/8/R5K1 w - -"));
    CHECK(g.tryMove(sq("a1"), sq("a8")));
    CHECK(g.status() == GameStatus::Checkmate);
    CHECK(g.winner() == Color::White);
}

TEST_CASE("check is reported but is not the end") {
    Game g(Board::fromFen("4k3/8/8/8/8/8/8/R3K3 w - -"));
    CHECK(g.tryMove(sq("a1"), sq("a8")));
    CHECK(g.status() == GameStatus::InProgress);
    CHECK(g.inCheck());
    CHECK(g.statusMessage() == "Black is in check");
}

TEST_CASE("stalemate") {
    Game g(Board::fromFen("7k/5Q2/6K1/8/8/8/8/8 b - -"));
    CHECK(g.status() == GameStatus::Stalemate);
}

TEST_CASE("insufficient material") {
    CHECK(Game(Board::fromFen("8/8/8/4k3/8/8/4K3/8 w - -")).status() == GameStatus::Draw);          // K v K
    CHECK(Game(Board::fromFen("8/8/8/4k3/8/8/4K3/7N w - -")).status() == GameStatus::Draw);         // K+N v K
    CHECK(Game(Board::fromFen("8/8/8/4k3/8/8/4K3/7R w - -")).status() == GameStatus::InProgress);   // K+R v K
}

TEST_CASE("fifty-move rule") {
    Game g(Board::fromFen("4k3/8/8/8/8/8/8/R3K3 w - - 99 60"));
    CHECK(g.status() == GameStatus::InProgress);
    CHECK(g.tryMove(sq("e1"), sq("e2")));                // the 100th half-move without pawn move or capture
    CHECK(g.status() == GameStatus::Draw);
}

TEST_CASE("threefold repetition") {
    Game g;
    for (int round = 0; round < 2; ++round) {
        CHECK(g.tryMove(sq("g1"), sq("f3")));
        CHECK(g.tryMove(sq("g8"), sq("f6")));
        CHECK(g.tryMove(sq("f3"), sq("g1")));
        CHECK(g.tryMove(sq("f6"), sq("g8")));
        if (round == 0) CHECK(g.status() == GameStatus::InProgress);   // the second time is still fine
    }
    CHECK(g.status() == GameStatus::Draw);                              // the third time ends the game
}

TEST_CASE("promotion defaults to a queen, and the caller can choose") {
    Game q(Board::fromFen("8/P7/8/8/8/8/8/k6K w - -"));
    CHECK(q.tryMove(sq("a7"), sq("a8")));
    CHECK(q.board().squares[0].type == PieceType::Queen);

    Game n(Board::fromFen("8/P7/8/8/8/8/8/k6K w - -"));
    CHECK(n.tryMove(sq("a7"), sq("a8"), PieceType::Knight));
    CHECK(n.board().squares[0].type == PieceType::Knight);
}