#include "ui.h"
#include "movegen.h"

#include <SFML/Graphics.hpp>

#include <optional>
#include <iostream>

namespace{

const int SQUARE = 80;
const unsigned BOARD_PX = 8*SQUARE;

const sf::Color LIGHT_SQ(240, 217, 181);
const sf::Color DARK_SQ(181, 136, 99);

const char32_t* glyphFor(PieceType t){
    switch(t){
        case PieceType::King : return U"\u265A";
        case PieceType::Queen : return U"\u265B";
        case PieceType::Rook : return U"\u265C";
        case PieceType::Bishop : return U"\u265D";
        case PieceType::Knight : return U"\u265E";
        case PieceType::Pawn : return U"\u265F";
        default : return U"";
    }
}

void drawBoard(sf::RenderWindow& window, const Board& board, const sf::Font& font) {
    sf::RectangleShape square(sf::Vector2f(SQUARE, SQUARE));
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            square.setPosition({static_cast<float>(c * SQUARE), static_cast<float>(r * SQUARE)});
            square.setFillColor((r + c) % 2 == 0 ? LIGHT_SQ : DARK_SQ);
            window.draw(square);

            const Piece p = board.at(r, c);
            if (p.empty()) continue;
            sf::Text text(font, sf::String(glyphFor(p.type)), 56);
            const bool white = p.color == Color::White;
            text.setFillColor(white ? sf::Color::White : sf::Color::Black);
            text.setOutlineColor(white ? sf::Color::Black : sf::Color::White);
            text.setOutlineThickness(1.5f);
            const sf::FloatRect b = text.getLocalBounds();           // centre the symbol in its square
            text.setOrigin({b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f});
            text.setPosition({c * SQUARE + SQUARE / 2.f, r * SQUARE + SQUARE / 2.f});
            window.draw(text);
        }
    }
}

void highlight(sf::RenderWindow& window, int idx, sf::Color color){
    sf::RectangleShape r(sf::Vector2f(SQUARE, SQUARE));
    r.setPosition({static_cast<float>((idx%8)*SQUARE), static_cast<float>((idx/8)*SQUARE)});
    r.setFillColor(color);
    window.draw(r);
}

void dot(sf::RenderWindow& window, int idx){
    sf::CircleShape c(SQUARE/6.f);
    c.setFillColor(sf::Color(0,0,0,90));
    c.setOrigin({SQUARE/6.f,SQUARE/6.f});
    c.setPosition({(idx%8)*SQUARE + SQUARE/2.f,(idx/8)*SQUARE + SQUARE/2.f});
    window.draw(c);
}


}

void runChess(Board board){
    sf::RenderWindow window(sf::VideoMode({BOARD_PX,BOARD_PX}), "CHESS");
    window.setFramerateLimit(60);

    sf::Font font;
    if(!font.openFromFile("C:/Windows/Fonts/seguisym.ttf")){
        std::cerr<<"Could not load font\n";
        return;
    }

    std::optional<int> selected;
    std::cerr << "runChess started\n";

    while(window.isOpen()){
        while(const std::optional event = window.pollEvent()){
            std::cerr << "event\n";
            if(event->is<sf::Event::Closed>()) {window.close();}
            else if(const auto* click = event->getIf<sf::Event::MouseButtonPressed>()){
                if(click->button != sf::Mouse::Button::Left) continue;
                const int col = click->position.x/SQUARE;
                const int row = click->position.y/SQUARE;
                if(row<0 || row>7 || col<0 || col>7) continue;
                const int idx = row*8+col;
                const Piece clicked = board.squares[idx];
                std::cout << "row " << row << " col " << col
          << " empty " << clicked.empty()
          << " white-to-move " << (board.sideToMove == Color::White)
          << " selected " << selected.has_value() << "\n";

                if(!clicked.empty() && clicked.color == board.sideToMove){
                    selected = idx;
                }else if(selected){
                    bool allowed = false;
                    for(const Move& m : generateLegal(board)){
                        if(m.from == *selected && m.to == idx){
                            board = applyMove(board , m);
                            break;
                        }
                    }
                    
                    
                    selected.reset();
                }
            }
        }

        window.clear();
        drawBoard(window, board, font);
        if(selected){
            highlight(window, *selected, sf::Color(255,255,0,120));
            for(const Move& m : generateLegal(board))
                if(m.from == *selected) dot(window,m.to);
        }
        window.display();
    }
}