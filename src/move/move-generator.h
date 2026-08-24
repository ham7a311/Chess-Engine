#pragma once
#include "../position/position.h"
#include <vector>
#include "move-representation.h"

class MoveGenerator {
    public: 
        std::vector<Move> generateMoves(Position&);
        void makeMove(Position& pos, const Move& m);
    private:
        std::vector<Move> generatePawnMoves(Position&, int square);
        std::vector<Move> generateKnightMoves(Position&, int square);
        std::vector<Move> generateBishopMoves(Position&, int square);
        std::vector<Move> generateRookMoves(Position&, int square);
        std::vector<Move> generateQueenMoves(Position&, int square);
        std::vector<Move> generateKingMoves(Position&, int square);
};