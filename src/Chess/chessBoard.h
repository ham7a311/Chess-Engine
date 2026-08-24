#pragma once

#include <SFML/Graphics.hpp>
#include "../position/position.h"

class ChessBoard {
public:
    ChessBoard();

    void draw(sf::RenderWindow& window, const Position& pos);

private:
    sf::Font font;
    float squareSize = 100.0f;

    sf::Color lightSquare{240, 217, 181};
    sf::Color darkSquare{181, 136, 99};
};
