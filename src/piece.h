#pragma once

enum class Color {White , Black} ;
enum class PieceType {None, Pawn, Knight, Bishop, Rook, Queen, King};

struct Piece{
    PieceType type = PieceType::None;
    Color color = Color::White;
    bool empty() const {return type = PieceType::None ;}
};

inline Color opposite(Color c){
    return c == Color::White ? Color::Black : Color::White ;
}