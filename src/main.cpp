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

            if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (pressed->button == sf::Mouse::Button::Left) {
                    board.onMousePressed(pos, window.mapPixelToCoords(pressed->position));
                }
            }

            if (const auto* released = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (released->button == sf::Mouse::Button::Left) {
                    board.onMouseReleased(pos, window.mapPixelToCoords(released->position));
                }
            }

            if (const auto* moved = event->getIf<sf::Event::MouseMoved>()) {
                board.onMouseMoved(window.mapPixelToCoords(moved->position));
            }
        }

        window.clear();

        board.draw(window, pos);

        window.display();
    }

    return 0;
}
