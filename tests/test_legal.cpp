#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

#include "movegen.h"

namespace {
Board make(const std::vector<std::string>& rows, Color side = Color::White) {
    return Board::fromRows(rows, side);
}
int countFrom(const std::vector<Move>& moves, int row, int col) {
    int n = 0;
    for (const Move& m : moves) if (m.from == row * 8 + col) ++n;
    return n;
}
}  // namespace

TEST_CASE("applyMove returns a new board") {
    Board b = Board::startingPosition();
    Board n = applyMove(b, Move{52, 36, PieceType::None});     // e2 to e4
    CHECK(n.squares[36].type == PieceType::Pawn);
    CHECK(n.squares[52].empty());
    CHECK(n.sideToMove == Color::Black);
    CHECK(b.squares[52].type == PieceType::Pawn);               // the original is untouched
    CHECK(b.sideToMove == Color::White);
}


TEST_CASE("pawn and knight give check") {
    std::vector<std::string> rows(8, "........");
    rows[6][4] = 'P';
    rows[5][3] = 'k';                                  // diagonally in front of the pawn
    CHECK(inCheck(make(rows), Color::Black));

    rows[5][3] = '.';
    rows[5][4] = 'k';                                  // straight ahead: not attacked
    CHECK_FALSE(inCheck(make(rows), Color::Black));

    std::vector<std::string> rows2(8, "........");
    rows2[4][4] = 'N';
    rows2[2][3] = 'k';
    CHECK(inCheck(make(rows2), Color::Black));
}


TEST_CASE("check detection") {
    std::vector<std::string> rows(8, "........");
    rows[0][4] = 'k';
    rows[7][4] = 'R';
    CHECK(inCheck(make(rows), Color::Black));
    rows[3][4] = 'P';                                            // blocks the rook
    CHECK_FALSE(inCheck(make(rows), Color::Black));
}

TEST_CASE("a pinned piece cannot move") {
    std::vector<std::string> rows(8, "........");
    rows[7][4] = 'K';
    rows[6][4] = 'N';                                            // pinned by the rook on the same file
    rows[0][4] = 'r';
    auto legal = generateLegal(make(rows));
    CHECK(countFrom(legal, 6, 4) == 0);
    CHECK(legal.size() == 4);                                    // only the king can move
}

TEST_CASE("a king cannot move next to the enemy king") {
    std::vector<std::string> rows(8, "........");
    rows[7][4] = 'K';
    rows[5][4] = 'k';
    CHECK(generateLegal(make(rows)).size() == 2);                // only (7,3) and (7,5)
}

TEST_CASE("perft from the starting position") {
    Board b = Board::startingPosition();
    CHECK(perft(b, 1) == 20);
    CHECK(perft(b, 2) == 400);
    CHECK(perft(b, 3) == 8902);
    CHECK(perft(b, 4) == 197281);
}