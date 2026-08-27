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

int ChessBoard::squareFromPoint(sf::Vector2f point) const {
    int file = static_cast<int>(point.x / squareSize);
    int rank = static_cast<int>(point.y / squareSize);
    if (file < 0 || file > 7 || rank < 0 || rank > 7) {
        return 64;
    }
    return rank * 8 + file;
}

void ChessBoard::selectSquare(Position& pos, int square) {
    selectedSquare = 64;
    legalMoves.clear();
    dragging = false;

    if (square < 0 || square > 63) {
        return;
    }
    if (pos.piece[square] == EMPTY || pos.color[square] != pos.sideToMove) {
        return;
    }

    selectedSquare = square;
    auto all = generator.generateMoves(pos);
    for (Move m : all) {
        if (m.from != square) {
            continue;
        }
        UndoState undo;
        if (generator.isLegalMove(pos, m, undo)) {
            legalMoves.push_back(m);
        }
    }
}

void ChessBoard::tryMoveTo(Position& pos, int to) {
    if (selectedSquare == 64 || to == 64 || to == selectedSquare) {
        return;
    }

    const Move* chosen = nullptr;
    for (const Move& m : legalMoves) {
        if (m.to != to) {
            continue;
        }
        if (chosen == nullptr ||
            ((m.flags & PROMOTION) && m.promotionPiece == QUEEN)) {
            chosen = &m;
        }
    }

    if (chosen) {
        UndoState undo;
        generator.makeMove(pos, *chosen, undo);
    }

    selectedSquare = 64;
    legalMoves.clear();
    dragging = false;
}

void ChessBoard::onMousePressed(Position& pos, sf::Vector2f point) {
    int square = squareFromPoint(point);

    if (selectedSquare != 64 && square != selectedSquare) {
        tryMoveTo(pos, square);
        if (selectedSquare == 64) {
            return;
        }
    }

    selectSquare(pos, square);
    if (selectedSquare != 64) {
        dragging = true;
        dragPos = point;
    }
}

void ChessBoard::onMouseReleased(Position& pos, sf::Vector2f point) {
    if (!dragging) {
        return;
    }

    int square = squareFromPoint(point);
    dragging = false;

    if (square != selectedSquare) {
        tryMoveTo(pos, square);
    }
}

void ChessBoard::onMouseMoved(sf::Vector2f point) {
    if (dragging) {
        dragPos = point;
    }
}

void ChessBoard::draw(sf::RenderWindow& window, const Position& pos) {
    sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));

    for (int sq = 0; sq < 64; sq++) {
        int file = sq % 8;
        int rank = sq / 8;

        square.setPosition(sf::Vector2f(file * squareSize, rank * squareSize));

        if (sq == selectedSquare)
            square.setFillColor(selectedColor);
        else if ((file + rank) % 2 == 0)
            square.setFillColor(lightSquare);
        else
            square.setFillColor(darkSquare);

        window.draw(square);
    }

    for (const Move& m : legalMoves) {
        int file = m.to % 8;
        int rank = m.to / 8;
        sf::CircleShape dot(16.f);
        dot.setFillColor(targetColor);
        dot.setPosition(sf::Vector2f(
            file * squareSize + squareSize / 2.f - 16.f,
            rank * squareSize + squareSize / 2.f - 16.f
        ));
        window.draw(dot);
    }

    for (int sq = 0; sq < 64; sq++) {
        if (pos.piece[sq] == EMPTY)
            continue;
        if (dragging && sq == selectedSquare)
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
        int file = sq % 8;
        int rank = sq / 8;
        pieceText.setPosition(sf::Vector2f(
            file * squareSize + (squareSize - bounds.size.x) / 2.f - bounds.position.x,
            rank * squareSize + (squareSize - bounds.size.y) / 2.f - bounds.position.y
        ));

        window.draw(pieceText);
    }

    if (dragging && selectedSquare <= 63 && pos.piece[selectedSquare] != EMPTY) {
        bool white = pos.color[selectedSquare] == WHITE;
        const char* glyph = " ";
        switch (pos.piece[selectedSquare]) {
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
            dragPos.x - squareSize / 2.f + (squareSize - bounds.size.x) / 2.f - bounds.position.x,
            dragPos.y - squareSize / 2.f + (squareSize - bounds.size.y) / 2.f - bounds.position.y
        ));
        window.draw(pieceText);
    }
}
