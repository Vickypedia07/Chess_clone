#include "board.h"
#include <cctype>

Board Board::startingPosition(){
    Board b;

    const PieceType back[8] = {PieceType::Rook,PieceType::Knight,PieceType::Bishop,PieceType::Queen,
    PieceType::King,PieceType::Bishop,PieceType::Knight,PieceType::Rook};


    for(int c=0 ; c<8 ; ++c){
        b.set(0,c,{back[c],Color::Black});
        b.set(1,c,{PieceType::Pawn,Color::Black});
        b.set(6,c,{PieceType::Pawn,Color::White});
        b.set(7,c,{back[c],Color::White});


    } return b;
}

Board Board::fromRows(const std::vector<std::string>& rows, Color side){
    Board b;
    b.sideToMove = side;

    for(int r=0 ; r<8 ; ++r){
        for( int c=0 ; c<8 ; ++c){
            const char ch = rows[r][c];
            if(ch == '.') continue;

            const Color color = std::isupper(static_cast<unsigned char>(ch)) ? Color::White : Color::Black;
            PieceType t = PieceType::None;
            
            switch(std::tolower(static_cast<unsigned char>(ch))){
                case 'p' : t = PieceType::Pawn; break;
                case 'n' : t = PieceType::Knight; break;
                case 'b' : t = PieceType::Bishop; break;
                case 'r' : t = PieceType::Rook; break;
                case 'q' : t = PieceType::Queen; break;
                case 'k' : t = PieceType::King; break;
                default : break;
            }
            b.set(r,c,{t,color});

        }
    }
    return b;

}