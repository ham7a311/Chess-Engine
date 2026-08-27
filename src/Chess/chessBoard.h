#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "../position/position.h"
#include "../move/move-generator.h"

class ChessBoard {
public:
    ChessBoard();

    void draw(sf::RenderWindow& window, const Position& pos);
    void onMousePressed(Position& pos, sf::Vector2f point);
    void onMouseReleased(Position& pos, sf::Vector2f point);
    void onMouseMoved(sf::Vector2f point);

private:
    int squareFromPoint(sf::Vector2f point) const;
    void selectSquare(Position& pos, int square);
    void tryMoveTo(Position& pos, int to);

    sf::Font font;
    float squareSize = 100.0f;
    MoveGenerator generator;
    std::vector<Move> legalMoves;

    int selectedSquare = 64;
    bool dragging = false;
    sf::Vector2f dragPos;

    sf::Color lightSquare{240, 217, 181};
    sf::Color darkSquare{181, 136, 99};
    sf::Color selectedColor{246, 246, 105};
    sf::Color targetColor{130, 151, 105};
};
