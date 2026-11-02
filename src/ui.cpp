#include "ui.h"

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


}

void runChess(Board board){
    sf::RenderWindow window(sf::VideoMode({BOARD_PX,BOARD_PX}), "CHESS");
    window.setFramerateLimit(60);

    sf::Font font;
    if(!font.openFromFile("C:/Windows/Fonts/seguisym.ttf")){
        std::cerr<<"Could not load font\n";
        return;
    }

    while(window.isOpen()){
        while(const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()) window.close();
        }

        window.clear();
        drawBoard(window, board, font);
        window.display();
    }
}