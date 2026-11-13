#include <catch2/catch_test_macros.hpp>
#include <vector>

#include "movegen.h"

namespace {
bool hasMove(const std::vector<Move>& moves, int from, int to) {
    for (const Move& m : moves) if (m.from == from && m.to == to) return true;
    return false;
}
}  // namespace

TEST_CASE("a double pawn push sets the en passant square") {
    Board n = applyMove(Board::startingPosition(), Move{52, 36, PieceType::None});   // e2 to e4
    CHECK(n.enPassant == 44);                                                          // e3
    Board m = applyMove(n, Move{12, 20, PieceType::None});                             // e7 to e6
    CHECK(m.enPassant == -1);
}

TEST_CASE("promotion") {
    Board b = Board::fromFen("8/P7/8/8/8/8/8/k6K w - -");
    int promos = 0;
    for (const Move& m : generateLegal(b))
        if (m.from == 8 && m.to == 0 && m.promotion != PieceType::None) ++promos;
    CHECK(promos == 4);
    Board n = applyMove(b, Move{8, 0, PieceType::Queen});
    CHECK(n.squares[0].type == PieceType::Queen);
    CHECK(n.squares[0].color == Color::White);
    CHECK(n.squares[8].empty());
}

TEST_CASE("en passant capture") {
    Board b = Board::fromFen("4k3/8/8/3pP3/8/8/8/4K3 w - d6");
    CHECK(hasMove(generateLegal(b), 28, 19));                  // e5 takes d6
    Board n = applyMove(b, Move{28, 19, PieceType::None});
    CHECK(n.squares[19].type == PieceType::Pawn);
    CHECK(n.squares[27].empty());                              // the black pawn on d5 is gone
    CHECK(n.squares[28].empty());
}

TEST_CASE("castling moves the king and the rook") {
    Board b = Board::fromFen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq -");
    auto moves = generateLegal(b);
    CHECK(hasMove(moves, 60, 62));
    CHECK(hasMove(moves, 60, 58));
    Board n = applyMove(b, Move{60, 62, PieceType::None});
    CHECK(n.squares[62].type == PieceType::King);
    CHECK(n.squares[61].type == PieceType::Rook);
    CHECK(n.squares[63].empty());
    CHECK_FALSE(n.castleWK);
    CHECK_FALSE(n.castleWQ);
}

TEST_CASE("castling is not allowed through an attacked square") {
    Board b = Board::fromFen("4k3/8/8/8/8/8/5r2/R3K2R w KQ -");
    auto moves = generateLegal(b);
    CHECK_FALSE(hasMove(moves, 60, 62));                       // f1 is attacked by the rook on f2
    CHECK(hasMove(moves, 60, 58));
}

TEST_CASE("castling is not allowed out of check") {
    Board b = Board::fromFen("4r1k1/8/8/8/8/8/8/R3K2R w KQ -");
    auto moves = generateLegal(b);
    CHECK_FALSE(hasMove(moves, 60, 62));
    CHECK_FALSE(hasMove(moves, 60, 58));
}

TEST_CASE("castling is not allowed with a piece in the way") {
    Board b = Board::fromFen("r3k2r/8/8/8/8/8/8/RN2K2R w KQkq -");
    auto moves = generateLegal(b);
    CHECK_FALSE(hasMove(moves, 60, 58));
    CHECK(hasMove(moves, 60, 62));
}

TEST_CASE("moving a rook removes only that castling right") {
    Board b = Board::fromFen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq -");
    Board n = applyMove(b, Move{63, 55, PieceType::None});     // h1 to h2
    CHECK_FALSE(n.castleWK);
    CHECK(n.castleWQ);
}

TEST_CASE("perft: Kiwipete (castling, en passant, pins)") {
    Board b = Board::fromFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    CHECK(perft(b, 1) == 48);
    CHECK(perft(b, 2) == 2039);
    CHECK(perft(b, 3) == 97862);
}

TEST_CASE("perft: position 3 (en passant and discovered checks)") {
    Board b = Board::fromFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    CHECK(perft(b, 1) == 14);
    CHECK(perft(b, 2) == 191);
    CHECK(perft(b, 3) == 2812);
    CHECK(perft(b, 4) == 43238);
}

TEST_CASE("perft: position 4 (promotions and castling)") {
    Board b = Board::fromFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -");
    CHECK(perft(b, 1) == 6);
    CHECK(perft(b, 2) == 264);
    CHECK(perft(b, 3) == 9467);
}

TEST_CASE("perft: start position depth 5", "[.slow]") {
    CHECK(perft(Board::startingPosition(), 5) == 4865609);
}