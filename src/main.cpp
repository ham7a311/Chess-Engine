#include <SFML/Graphics.hpp>

#include "position/position.h"
#include "Chess/chessBoard.h"

int main() {

    Position pos;

    sf::RenderWindow window(
        sf::VideoMode({800, 800}),
        "C++ Chess"
    );

    ChessBoard board;

    while (window.isOpen()) {

        while (auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();

        board.draw(window, pos);

        window.display();
    }

    return 0;
}
