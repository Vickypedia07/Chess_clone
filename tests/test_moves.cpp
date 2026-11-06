#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

#include "movegen.h"

namespace {

// Put one piece on an otherwise empty board and list White's pseudo-legal moves.
std::vector<Move> movesFor(char piece, int row, int col) {
    std::vector<std::string> rows(8, "........");
    rows[row][col] = piece;
    return generatePseudoLegal(Board::fromRows(rows, Color::White));
}

int countFrom(const std::vector<Move>& moves, int row, int col) {
    int n = 0;
    for (const Move& m : moves) if (m.from == row * 8 + col) ++n;
    return n;
}

}  // namespace

TEST_CASE("knight moves") {
    CHECK(movesFor('N', 7, 0).size() == 2);   // corner
    CHECK(movesFor('N', 4, 4).size() == 8);   // centre
}

TEST_CASE("king moves") {
    CHECK(movesFor('K', 4, 4).size() == 8);
    CHECK(movesFor('K', 7, 0).size() == 3);   // corner
}

TEST_CASE("rook moves") {
    CHECK(movesFor('R', 4, 4).size() == 14);
    CHECK(movesFor('R', 7, 0).size() == 14);
}

TEST_CASE("bishop and queen moves") {
    CHECK(movesFor('B', 4, 3).size() == 13);   // d4
    CHECK(movesFor('Q', 4, 3).size() == 27);
}

TEST_CASE("pawn moves") {
    CHECK(movesFor('P', 6, 4).size() == 2);    // start row: one or two forward
    CHECK(movesFor('P', 5, 4).size() == 1);    // already moved: one forward
}

TEST_CASE("pawn blocked and pawn captures") {
    std::vector<std::string> rows(8, "........");
    rows[6][4] = 'P';
    rows[5][4] = 'p';                           // enemy directly in front: pawn cannot move
    CHECK(generatePseudoLegal(Board::fromRows(rows, Color::White)).size() == 0);

    rows[5][4] = '.';
    rows[5][3] = 'p';
    rows[5][5] = 'p';                           // two enemies on the diagonals
    CHECK(generatePseudoLegal(Board::fromRows(rows, Color::White)).size() == 4);   // 2 forward + 2 captures
}

TEST_CASE("sliding pieces are blocked") {
    std::vector<std::string> rows(8, "........");
    rows[4][4] = 'R';
    rows[3][4] = 'P';                           // own piece above the rook
    CHECK(countFrom(generatePseudoLegal(Board::fromRows(rows, Color::White)), 4, 4) == 10);

    rows[3][4] = 'p';                           // enemy piece above: capture it, but not go beyond
    CHECK(countFrom(generatePseudoLegal(Board::fromRows(rows, Color::White)), 4, 4) == 11);
}

TEST_CASE("starting position has 20 moves for each side") {
    Board b = Board::startingPosition();
    CHECK(generatePseudoLegal(b).size() == 20);
    b.sideToMove = Color::Black;
    CHECK(generatePseudoLegal(b).size() == 20);
}