#include "chessBoard.h"

#include <stdexcept>
#include <string>
#include <vector>

ChessBoard::ChessBoard() {
    const std::vector<std::string> fontPaths = {
        "assets/NotoSansSymbols2-Regular.ttf",
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/System/Library/Fonts/Apple Symbols.ttf",
        "/Library/Fonts/Arial Unicode.ttf",
    };

    bool loaded = false;
    for (const auto& path : fontPaths) {
        if (font.openFromFile(path)) {
            loaded = true;
            break;
        }
    }

    if (!loaded) {
        throw std::runtime_error(
            "Could not load a font with chess glyphs. "
            "Place NotoSansSymbols2-Regular.ttf in assets/"
        );
    }
}

void ChessBoard::draw(sf::RenderWindow& window, const Position& pos) {
    sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));

    for (int sq = 0; sq < 64; sq++) {
        int file = sq % 8;
        int rank = sq / 8;

        square.setPosition(sf::Vector2f(file * squareSize, rank * squareSize));

        if ((file + rank) % 2 == 0)
            square.setFillColor(lightSquare);
        else
            square.setFillColor(darkSquare);

        window.draw(square);

        if (pos.piece[sq] == EMPTY)
            continue;

        bool white = pos.color[sq] == WHITE;
        const char* glyph = " ";

        switch (pos.piece[sq]) {
            case KING:   glyph = white ? "♔" : "♚"; break;
            case QUEEN:  glyph = white ? "♕" : "♛"; break;
            case ROOK:   glyph = white ? "♖" : "♜"; break;
            case BISHOP: glyph = white ? "♗" : "♝"; break;
            case KNIGHT: glyph = white ? "♘" : "♞"; break;
            case PAWN:   glyph = white ? "♙" : "♟"; break;
        }

        sf::Text pieceText(font);
        pieceText.setCharacterSize(72);
        pieceText.setString(sf::String::fromUtf8(glyph, glyph + std::char_traits<char>::length(glyph)));
        pieceText.setFillColor(white ? sf::Color::White : sf::Color::Black);
        pieceText.setOutlineColor(sf::Color::Black);
        pieceText.setOutlineThickness(white ? 2.f : 0.f);

        auto bounds = pieceText.getLocalBounds();
        pieceText.setPosition(sf::Vector2f(
            file * squareSize + (squareSize - bounds.size.x) / 2.f - bounds.position.x,
            rank * squareSize + (squareSize - bounds.size.y) / 2.f - bounds.position.y
        ));

        window.draw(pieceText);
    }
}
