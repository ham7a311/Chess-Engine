#include "move-generator.h"

std::vector<Move> MoveGenerator::generatePawnMoves(Position& pos, int square) {
    std::vector<Move> v;

    // next move is current square number + 8 and -8 for white for going one square up and +16 and -16 for white for two squares up(first move only)
    if(pos.sideToMove == WHITE) {

        // calc destination, check if not empty and add to vector
        int oneSquare = square - 8;
        int twoSquare = square - 16;

        // one square moves

        if(0 <= oneSquare && oneSquare <= 63 && pos.piece[oneSquare] == EMPTY) {
            // promotion logic
        // for white, if a pawn is on squares 8-15 means the white pawn is one move away from promotion, 0-7 are promotion squares
            if(0 <= oneSquare && oneSquare <= 7) {
                // 4 possible promotions, create 4 Move objects
                Move m;
                m.from = square;
                m.to = oneSquare;
                m.flags = PROMOTION;
                m.promotionPiece = QUEEN;
                v.push_back(m);

                Move m2;
                m2.from = square;
                m2.to = oneSquare;
                m2.flags = PROMOTION;
                m2.promotionPiece = ROOK;
                v.push_back(m2);

                Move m3;
                m3.from = square;
                m3.to = oneSquare;
                m3.flags = PROMOTION;
                m3.promotionPiece = BISHOP;
                v.push_back(m3);

                Move m4;
                m4.from = square;
                m4.to = oneSquare;
                m4.flags = PROMOTION;
                m4.promotionPiece = KNIGHT;
                v.push_back(m4);

            } else {
                Move m;
                m.from = square;
                m.to = oneSquare;
                v.push_back(m);
            }
            
        }

        // two square moves(starting position only: 48-55)
        if(0 <= twoSquare && twoSquare <= 63 && pos.piece[twoSquare] == EMPTY && pos.piece[oneSquare] == EMPTY && 48 <= square && square <= 55) {
            
            Move m;
            m.from = square;
            m.to = twoSquare;
            v.push_back(m);
            
        }

        // capture logic
        // white can only capture square - 7 and square - 9 , ONLY and ONLY IF there is a black piece on these squares
        // add capture to the flags when move is done

        int file = square % 8;  // this helps us to check if pawn is on the edge or not(if on edge only 1 capture is possible not 2 as usual)

        if(file > 0) {

            int capture = square - 9;
            if(pos.color[capture] != pos.sideToMove && pos.piece[capture] != EMPTY) {
                Move m;
                m.from = square;
                m.to = capture;
                m.flags = CAPTURE;
                v.push_back(m);
            }

        }
        
        if(file < 7) {

            int capture = square - 7;
            if(pos.color[capture] != pos.sideToMove && pos.piece[capture] != EMPTY) {
                Move m;
                m.from = square;
                m.to = capture;
                m.flags = CAPTURE;
                v.push_back(m);
            }
        }

        
    } else {
        // calc destination, check if not empty and add to vector
        int oneSquare = square + 8;
        int twoSquare = square + 16;

        if(0 <= oneSquare && oneSquare <= 63 && pos.piece[oneSquare] == EMPTY) {
            // promotion logic
            // for black, if a pawn is on squares 48-55 means the white pawn is one move away from promotion, 56-63 are promotion squares
            if(56 <= oneSquare && oneSquare <= 63) {
                
                 // 4 possible promotions, create 4 Move objects
                 Move m;
                 m.from = square;
                 m.to = oneSquare;
                 m.flags = PROMOTION;
                 m.promotionPiece = QUEEN;
                 v.push_back(m);
 
                 Move m2;
                 m2.from = square;
                 m2.to = oneSquare;
                 m2.flags = PROMOTION;
                 m2.promotionPiece = ROOK;
                 v.push_back(m2);
 
                 Move m3;
                 m3.from = square;
                 m3.to = oneSquare;
                 m3.flags = PROMOTION;
                 m3.promotionPiece = BISHOP;
                 v.push_back(m3);
 
                 Move m4;
                 m4.from = square;
                 m4.to = oneSquare;
                 m4.flags = PROMOTION;
                 m4.promotionPiece = KNIGHT;
                 v.push_back(m4);

            } else {
                Move m;
                m.from = square;
                m.to = oneSquare;
                v.push_back(m);
            }
            
        }

         // two square moves(starting position only: 8-15)
         if(0 <= twoSquare && twoSquare <= 63 && pos.piece[twoSquare] == EMPTY && pos.piece[oneSquare] == EMPTY && 8 <= square && square <= 15) {
            
            Move m;
            m.from = square;
            m.to = twoSquare;
            v.push_back(m);
            
        }

         // capture logic
        // white can only capture square - 7 and square - 9 , ONLY and ONLY IF there is a black piece on these squares
        // add capture to the flags when move is done

        int file = square % 8;  // this helps us to check if pawn is on the edge or not(if on edge only 1 capture is possible not 2 as usual)

        if(file > 0) {

            int capture = square + 7;
            if(pos.color[capture] != pos.sideToMove && pos.piece[capture] != EMPTY) {
                Move m;
                m.from = square;
                m.to = capture;
                m.flags = CAPTURE;
                v.push_back(m);
            }

        }
        
        if(file < 7) {

            int capture = square + 9;
            if(pos.color[capture] != pos.sideToMove && pos.piece[capture] != EMPTY) {
                Move m;
                m.from = square;
                m.to = capture;
                m.flags = CAPTURE;
                v.push_back(m);
            }
        }
        
    }

    return v;
}

std::vector<Move> MoveGenerator::generateMoves(Position& pos) {

    

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


