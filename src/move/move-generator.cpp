#include "move-generator.h"


std::vector<Move> MoveGenerator::generateMoves(Position& pos) {

    std::vector<Move> generatePawnMoves(Position&, int square) {
        
    }

    std::vector<Move> v;

    for(int i = 0; i < 64; i++) {

        if(pos.piece[i] != EMPTY && pos.color[i] == pos.sideToMove) {
            // determine type of piece
            // generate all possible moves of that piece then push them to the vector

            switch (pos.piece[i]){
                case PAWN: {
                    auto pawnMoves = generatePawnMoves(pos, i);
                    v.insert(v.end(), pawnMoves.begin(), pawnMoves.end());
                    break;
                }
                case KNIGHT: {
                    auto knightMoves = generateKnightMoves(pos, i);
                    v.insert(v.end(), knightMoves.begin(), knightMoves.end());
                    break;
                }
                case BISHOP: {
                    auto bishopMoves =generateBishopMoves(pos, i);
                    v.insert(v.end(), bishopMoves.begin(), bishopMoves.end());
                    break;
                }
                case ROOK: {
                    auto rookMoves =generateRookMoves(pos, i);
                    v.insert(v.end(), rookMoves.begin(), rookMoves.end());
                    break;
                }
                case QUEEN: {
                    auto queenMoves = generateQueenMoves(pos, i);
                    v.insert(v.end(), queenMoves.begin(), queenMoves.end());
                    break;
                }
                case KING: {
                   auto kingMoves =  generateKingMoves(pos, i);
                   v.insert(v.end(), kingMoves.begin(), kingMoves.end());
                    break;
                }
            }

        }

    }

    return v;
}


