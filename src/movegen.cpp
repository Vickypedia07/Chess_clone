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

void addPawn(const Board& b, std::vector<Move>& moves, int r, int c){
    const Color me = b.at(r,c).color;
    const int dir = (me == Color::White) ? -1 : 1; //White pawns move up, Black pawns move down
    const int startRow = (me == Color::White) ? 6 : 1; // starting row for pawns

    if(onBoard(r+dir,c) && b.at(r+dir,c).empty()){
        addMove(moves, r , c , r+dir , c);
        if(r == startRow && b.at(r+2*dir,c).empty()){
            addMove(moves , r , c , r+2*dir , c); // double move from starting position
        }
    }

    for(int dc = -1 ; dc <= 1 ; dc+= 2){
        const int tr = r+dir , tc = c+dc;
        if(!onBoard(tr,tc)) continue;
        const Piece target = b.at(tr,tc);
        if(!target.empty() && target.color != me) addMove(moves , r , c , tr , tc); //diagonal capture for pawns

    }
}

}

std::vector<Move> generatePseudoLegal(const Board& b){
    std::vector<Move> moves;
    for(int r = 0 ; r < 8 ; ++r){
        for(int c = 0 ; c < 8 ; ++c){
            const Piece p = b.at(r,c);
            if(p.empty() || p.color != b.sideToMove) continue;
            switch(p.type){
                case PieceType::Knight : for(const auto& d : KNIGHT) addStep(b,moves,r,c,d[0],d[1]); break;
                case PieceType::King : for(const auto& d : KING) addStep(b,moves,r,c,d[0],d[1]); break;
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

Board applyMove(const Board& b, Move m){
    Board next = b;
    next.squares[m.to] = next.squares[m.from];
    next.squares[m.from] = Piece{};
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
    for(const Move& m : generatePseudoLegal(opp))
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
