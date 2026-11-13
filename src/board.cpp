#include "board.h"
#include <cctype>
#include <sstream>

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
    b.castleWK = b.castleWQ = b.castleBK = b.castleBQ = false;

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

Board Board::fromFen(const std::string& fen){
    Board b;
    b.castleWK = b.castleWQ = b.castleBK = b.castleBQ = false;
    std::istringstream in(fen);
    std::string placement, side, castling, ep;
    in >> placement >> side >> castling >> ep;

    int r=0, c=0;
    for(char ch : placement){
        if(ch == '/'){ ++r; c=0 ;}
        else if(std::isdigit(static_cast<unsigned char>(ch))) c+=ch-'0';
        else{
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
            ++c;
        }
    }

    b.sideToMove = (side =="b") ? Color::Black : Color::White;
    for(char ch : castling){
        if(ch == 'K') b.castleWK = true;
        if(ch == 'Q') b.castleWQ = true;
        if(ch == 'k') b.castleBK = true;
        if(ch == 'q') b.castleBQ = true;
    }

    if(ep.size() == 2 && ep != "-") b.enPassant = (8-(ep[1]-'0')) * 8 + (ep[0]-'a');
    return b;

}