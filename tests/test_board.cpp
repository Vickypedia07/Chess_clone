#include <catch2/catch_test_macros.hpp>
#include "board.h"

TEST_CASE("starting position") {
    Board b = Board::startingPosition();
    int count = 0;
    for (const auto& p : b.squares) if (!p.empty()) ++count;
    CHECK(count == 32);
    CHECK(b.at(7, 4).type == PieceType::King);
    CHECK(b.at(7, 4).color == Color::White);
    CHECK(b.at(0, 3).type == PieceType::Queen);
    CHECK(b.at(0, 3).color == Color::Black);
}