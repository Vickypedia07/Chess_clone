#include "movegen.h"

namespace{

const int KNIGHT[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
const int KING[8][2] =  {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
const int ROOK_DIRS[4][2] =  {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
const int BISHOP_DIRS[4][2] =  {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

bool onBoard(int r, int c){
    return r>=0 && r<8 && c>=0 && c<8;
}

void addMove(std::vector<Move>& moves, int r, int c, int tr, int tc){
    moves.push_back(Move{r*8+c, tr*8+tc, PieceType::None});
}

//adds move for Knight and King to move in one direction, one step at a time
void addStep(const Board& b, std::vector<Move>& moves, int r, int c, int dr, int dc){
    const int tr = r+dr , tc = c+dc;
    if(!onBoard(tr,tc)) return;
    const Piece target = b.at(tr,tc);
    if(target.empty() || target.color != b.at(r,c).color) addMove(moves,r,c,tr,tc);
}

//adds moves for Rook, Bishop and Queen to move in one direction, multiple steps at a time
void addSlide(const Board& b, std::vector<Move>& moves , int r , int c , int dr , int dc){
    const Color me = b.at(r,c).color;
    int tr = r + dr , tc = c + dc;
    while(onBoard(tr , tc)){
        const Piece target = b.at(tr , tc);
        if(target.empty()){
            addMove(moves , r , c, tr , tc); // slides the piece until it hits piece or the edge of the board
        }else{
            if(target.color != me) addMove(moves , r , c , tr , tc); // if it hits an enemy piece, add the move and stop sliding
            break;
        }
        tc += dc; tr += dr;
    }
}

void addPawnMove(std::vector<Move>& moves, int r, int c, int tr, int tc ){
    if(tr == 0 || tr == 7){
        const PieceType options[4] = {PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight};
        for(PieceType t : options) moves.push_back(Move{r*8+c, tr*8+tc,t});

    }else{
        addMove(moves, r, c, tr, tc);
    }
}

void addPawn(const Board& b, std::vector<Move>& moves, int r, int c){
    const Color me = b.at(r,c).color;
    const int dir = (me == Color::White) ? -1 : 1; //White pawns move up, Black pawns move down
    const int startRow = (me == Color::White) ? 6 : 1; // starting row for pawns

    if(onBoard(r+dir,c) && b.at(r+dir,c).empty()){
        addPawnMove(moves, r , c , r+dir , c);
        if(r == startRow && b.at(r+2*dir,c).empty()){
            addMove(moves , r , c , r+2*dir , c); // double move from starting position
        }
    }

    for(int dc = -1 ; dc <= 1 ; dc+= 2){
        const int tr = r+dir , tc = c+dc;
        if(!onBoard(tr,tc)) continue;
        const Piece target = b.at(tr,tc);
        if(!target.empty() && target.color != me) addPawnMove(moves , r , c , tr , tc); //diagonal capture for pawns
        else if(target.empty() && tr*8+tc == b.enPassant) addMove(moves, r, c, tr, tc); //en passant capture

    }
}

bool squareAttacked(const Board& b, int sq, Color by) {
    Board opp = b;
    opp.sideToMove = by;                                   
    for (const Move& m : generatePseudoLegal(opp, false))  
        if (m.to == sq) return true;
    return false;
}

void addCastling(const Board& b, std::vector<Move>& moves, Color me) {
    const bool white = (me == Color::White);
    const int row = white ? 7 : 0;
    const Color enemy = opposite(me);
    const Piece king = b.at(row, 4);
    if (king.type != PieceType::King || king.color != me) return;
    if (squareAttacked(b, row * 8 + 4, enemy)) return;     // cannot castle out of check

    const bool kingSide = white ? b.castleWK : b.castleBK;
    const bool queenSide = white ? b.castleWQ : b.castleBQ;
    const Piece rookK = b.at(row, 7), rookQ = b.at(row, 0);

    if (kingSide && rookK.type == PieceType::Rook && rookK.color == me &&
        b.at(row, 5).empty() && b.at(row, 6).empty() &&
        !squareAttacked(b, row * 8 + 5, enemy) && !squareAttacked(b, row * 8 + 6, enemy))
        addMove(moves, row, 4, row, 6);

    if (queenSide && rookQ.type == PieceType::Rook && rookQ.color == me &&
        b.at(row, 1).empty() && b.at(row, 2).empty() && b.at(row, 3).empty() &&
        !squareAttacked(b, row * 8 + 3, enemy) && !squareAttacked(b, row * 8 + 2, enemy))
        addMove(moves, row, 4, row, 2);
}

}

std::vector<Move> generatePseudoLegal(const Board& b, bool withCastling){
    std::vector<Move> moves;
    for(int r = 0 ; r < 8 ; ++r){
        for(int c = 0 ; c < 8 ; ++c){
            const Piece p = b.at(r,c);
            if(p.empty() || p.color != b.sideToMove) continue;
            switch(p.type){
                case PieceType::Knight : for(const auto& d : KNIGHT) addStep(b,moves,r,c,d[0],d[1]); break;
                case PieceType::King : for(const auto& d : KING) addStep(b,moves,r,c,d[0],d[1]);
                                        if(withCastling) addCastling(b,moves,p.color); break;
                case PieceType::Rook : for(const auto& d : ROOK_DIRS) addSlide(b,moves,r,c,d[0],d[1]); break;
                case PieceType::Bishop : for(const auto& d : BISHOP_DIRS) addSlide(b,moves,r,c,d[0],d[1]); break;
                case PieceType::Queen : for(const auto& d : ROOK_DIRS) addSlide(b,moves,r,c,d[0],d[1]);
                                        for(const auto& d : BISHOP_DIRS) addSlide(b,moves,r,c,d[0],d[1]); 
                                        break;
                case PieceType::Pawn : addPawn(b,moves,r,c); break;
                default : break;

            }
        }
    }
    return moves;
}

Board applyMove(const Board& b, Move m) {
    Board next = b;
    const Piece moving = b.squares[m.from];
    const int fr = m.from / 8, fc = m.from % 8, tr = m.to / 8, tc = m.to % 8;

    // en passant
    if (moving.type == PieceType::Pawn && m.to == b.enPassant && fc != tc)
        next.squares[fr * 8 + tc] = Piece{};               // remove the pawn that just moved past

    // castling
    if (moving.type == PieceType::King && (tc - fc == 2 || fc - tc == 2)) {
        const int rookFrom = fr * 8 + (tc > fc ? 7 : 0);
        const int rookTo = fr * 8 + (tc > fc ? 5 : 3);
        next.squares[rookTo] = next.squares[rookFrom];
        next.squares[rookFrom] = Piece{};
    }

    next.squares[m.to] = moving;
    next.squares[m.from] = Piece{};
    if (m.promotion != PieceType::None) next.squares[m.to].type = m.promotion;

    // en passant square: set only right after a two-square pawn push
    next.enPassant = -1;
    if (moving.type == PieceType::Pawn && (tr - fr == 2 || fr - tr == 2))
        next.enPassant = ((fr + tr) / 2) * 8 + fc;

    // castling rights: lost when a king or rook moves from its start square, or a rook is captured
    auto touch = [&next](int sq) {
        if (sq == 60) next.castleWK = next.castleWQ = false;   
        if (sq == 4) next.castleBK = next.castleBQ = false;    
        if (sq == 63) next.castleWK = false;
        if (sq == 56) next.castleWQ = false;
        if (sq == 7) next.castleBK = false;
        if (sq == 0) next.castleBQ = false;
    };
    touch(m.from);
    touch(m.to);

    next.sideToMove = opposite(b.sideToMove);
    return next;
}

bool inCheck(const Board& b , Color c){
    int kingSquare = -1;
    for(int i=0 ; i<64 ; ++i){
        if(b.squares[i].type == PieceType::King && b.squares[i].color == c) kingSquare = i;

    }if(kingSquare < 0) return false;

    Board opp = b;
    opp.sideToMove = opposite(c);
    for(const Move& m : generatePseudoLegal(opp,false))
        if(m.to == kingSquare) return true;
    
    return false;
}

std::vector<Move> generateLegal(const Board& b){
    std::vector<Move> legal;
    for(const Move& m : generatePseudoLegal(b))
        if(!inCheck(applyMove(b,m),b.sideToMove)) legal.push_back(m);
    
    return legal;
}

long long perft(const Board& b, int depth){
    if(depth == 0) return 1;
    long long total = 0;
    for(const Move& m : generateLegal(b)) total += perft(applyMove(b,m),depth-1);
    
    return total;
}

std::string squareName(int sq){
    std::string s;
    s += static_cast<char>('a' + sq%8);
    s += static_cast<char>('8' - sq/8);
    return s;
}

std::string moveName(const Move& m){
    std::string s = squareName(m.from) + squareName(m.to);
    switch(m.promotion){
        case PieceType::Knight : s += 'n'; break;
        case PieceType::Bishop : s += 'b'; break;
        case PieceType::Rook : s += 'r'; break;
        case PieceType::Queen : s += 'q'; break;
        default : break;
        }
    
    
    return s;
}
