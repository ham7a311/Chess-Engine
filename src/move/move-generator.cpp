#include "move-generator.h"
#include <utility>

void makeMove(Position& pos, const Move& m, UndoState& undo) {

    int movingPiece = pos.piece[m.from];
    int movingColor = pos.color[m.from];

    const int originalMovingPiece = movingPiece;

    // --------------------------------------------------
    // Save previous state for unmakeMove()
    // --------------------------------------------------

    undo.previousEnPassantSquare = pos.enPassantSquare;
    undo.previousCastlingRights = pos.castlingRights;
    undo.previousHalfMoveClock = pos.halfmoveClock;
    undo.previousFullMoveNumber = pos.fullmoveNumber;

    // Determine where the captured piece is.
    // Normally it is on m.to.
    // For en passant, it is behind m.to.
    undo.capturedSquare = m.to;

    if (m.flags & EN_PASSANT) {

        if (movingColor == WHITE) {
            undo.capturedSquare = m.to + 8;
        } else {
            undo.capturedSquare = m.to - 8;
        }
    }

    // IMPORTANT:
    // Save the captured piece BEFORE removing it.
    undo.capturedPiece = pos.piece[undo.capturedSquare];
    undo.capturedColor = pos.color[undo.capturedSquare];

    // --------------------------------------------------
    // Reset en passant square
    // --------------------------------------------------

    pos.enPassantSquare = 64;

    // --------------------------------------------------
    // Castling rights
    // --------------------------------------------------

    if (movingPiece == KING) {

        if (movingColor == WHITE) {
            pos.castlingRights &= ~WHITE_KINGSIDE;
            pos.castlingRights &= ~WHITE_QUEENSIDE;
        } else {
            pos.castlingRights &= ~BLACK_KINGSIDE;
            pos.castlingRights &= ~BLACK_QUEENSIDE;
        }
    }

    if (movingPiece == ROOK) {

        if (movingColor == WHITE) {

            if (m.from == 56) {
                pos.castlingRights &= ~WHITE_QUEENSIDE;
            } else if (m.from == 63) {
                pos.castlingRights &= ~WHITE_KINGSIDE;
            }

        } else {

            if (m.from == 0) {
                pos.castlingRights &= ~BLACK_QUEENSIDE;
            } else if (m.from == 7) {
                pos.castlingRights &= ~BLACK_KINGSIDE;
            }
        }
    }

    // If a rook is captured on its original square,
    // remove that side's castling right.

    if (pos.piece[m.to] == ROOK && pos.color[m.to] == WHITE) {

        if (m.to == 56) {
            pos.castlingRights &= ~WHITE_QUEENSIDE;
        } else if (m.to == 63) {
            pos.castlingRights &= ~WHITE_KINGSIDE;
        }
    }

    if (pos.piece[m.to] == ROOK && pos.color[m.to] == BLACK) {

        if (m.to == 0) {
            pos.castlingRights &= ~BLACK_QUEENSIDE;
        } else if (m.to == 7) {
            pos.castlingRights &= ~BLACK_KINGSIDE;
        }
    }

    // --------------------------------------------------
    // Remove captured piece
    // --------------------------------------------------

    if (m.flags & EN_PASSANT) {

        pos.piece[undo.capturedSquare] = EMPTY;
        pos.color[undo.capturedSquare] = EMPTY;

    } else if (m.flags & CAPTURE) {

        pos.piece[m.to] = EMPTY;
        pos.color[m.to] = EMPTY;
    }

    // --------------------------------------------------
    // Remove moving piece from original square
    // --------------------------------------------------

    pos.piece[m.from] = EMPTY;
    pos.color[m.from] = EMPTY;

    // --------------------------------------------------
    // Promotion
    // --------------------------------------------------

    if (m.flags & PROMOTION) {
        movingPiece = m.promotionPiece;
    }

    // --------------------------------------------------
    // Castling
    // --------------------------------------------------

    if ((m.flags & CASTLING) && movingPiece == KING) {

        if (m.from == 4 && m.to == 2) {

            // Black queenside: e8 -> c8
            // Rook: a8 -> d8

            pos.piece[0] = EMPTY;
            pos.color[0] = EMPTY;

            pos.piece[3] = ROOK;
            pos.color[3] = BLACK;

        } 
        else if (m.from == 4 && m.to == 6) {

            // Black kingside: e8 -> g8
            // Rook: h8 -> f8

            pos.piece[7] = EMPTY;
            pos.color[7] = EMPTY;

            pos.piece[5] = ROOK;
            pos.color[5] = BLACK;

        } 
        else if (m.from == 60 && m.to == 58) {

            // White queenside: e1 -> c1
            // Rook: a1 -> d1

            pos.piece[56] = EMPTY;
            pos.color[56] = EMPTY;

            pos.piece[59] = ROOK;
            pos.color[59] = WHITE;

        } 
        else if (m.from == 60 && m.to == 62) {

            // White kingside: e1 -> g1
            // Rook: h1 -> f1

            pos.piece[63] = EMPTY;
            pos.color[63] = EMPTY;

            pos.piece[61] = ROOK;
            pos.color[61] = WHITE;
        }
    }

    // --------------------------------------------------
    // Put moving piece on destination
    // --------------------------------------------------

    pos.piece[m.to] = movingPiece;
    pos.color[m.to] = movingColor;

    // --------------------------------------------------
    // En passant logic
    // --------------------------------------------------

    if (originalMovingPiece == PAWN &&
        abs(m.from - m.to) == 16) {

        pos.enPassantSquare = (m.from + m.to) / 2;
    }

    // --------------------------------------------------
    // Halfmove clock
    // --------------------------------------------------

    if (originalMovingPiece == PAWN || (m.flags & CAPTURE)) {
        pos.halfmoveClock = 0;
    } else {
        pos.halfmoveClock++;
    }

    // --------------------------------------------------
    // Fullmove number
    // --------------------------------------------------

    if (pos.sideToMove == BLACK) {
        pos.fullmoveNumber++;
    }

    // --------------------------------------------------
    // Switch side
    // --------------------------------------------------

    pos.sideToMove = !pos.sideToMove;
}



void unmakeMove(Position& pos, const Move& m, UndoState& undo) {
    // Reverse makeMove() logic

    pos.sideToMove = !pos.sideToMove;

    pos.fullmoveNumber = undo.previousFullMoveNumber;
    pos.halfmoveClock = undo.previousHalfMoveClock;
    pos.castlingRights = undo.previousCastlingRights;
    pos.enPassantSquare = undo.previousEnPassantSquare;

    // The piece currently on 'to' is the piece that moved.
    int movingPiece = pos.piece[m.to];
    int movingColor = pos.color[m.to];

    // Remove moving piece from destination.
    pos.piece[m.to] = EMPTY;
    pos.color[m.to] = EMPTY;

    // If it was a promotion, restore the original pawn.
    if (m.flags & PROMOTION) {
        movingPiece = PAWN;
    }

    // Restore captured piece.
    if (m.flags & CAPTURE) {
        pos.piece[undo.capturedSquare] = undo.capturedPiece;
        pos.color[undo.capturedSquare] = undo.capturedColor;
    }

    // Restore rook when undoing castling.
    if (m.flags & CASTLING) {
        // Black queenside: rook d8 -> a8
        if (m.from == 4 && m.to == 2) {

            // Black queenside: e8 -> c8
            // Rook: a8 -> d8

            pos.piece[0] = ROOK;
            pos.color[0] = BLACK;

            pos.piece[3] = EMPTY;
            pos.color[3] = EMPTY;

        } 
        else if (m.from == 4 && m.to == 6) {

            // Black kingside: e8 -> g8
            // Rook: h8 -> f8

            pos.piece[7] = ROOK;
            pos.color[7] = BLACK;

            pos.piece[5] = EMPTY;
            pos.color[5] = EMPTY;

        } 
        else if (m.from == 60 && m.to == 58) {

            // White queenside: e1 -> c1
            // Rook: a1 -> d1

            pos.piece[56] = ROOK;
            pos.color[56] = WHITE;

            pos.piece[59] = EMPTY;
            pos.color[59] = EMPTY;

        } 
        else if (m.from == 60 && m.to == 62) {

            // White kingside: e1 -> g1
            // Rook: h1 -> f1

            pos.piece[63] = ROOK;
            pos.color[63] = WHITE;

            pos.piece[61] = EMPTY;
            pos.color[61] = EMPTY;
        }
    }

    // Put moving piece back on its original square.
    pos.piece[m.from] = movingPiece;
    pos.color[m.from] = movingColor;
}



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
            if(0 <= capture && capture <= 63 && pos.color[capture] != pos.sideToMove && pos.piece[capture] != EMPTY) {
                if(0 <= capture && capture <= 7){

                    // 4 possible promotions, create 4 Move objects
                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = PROMOTION | CAPTURE;
                    m.promotionPiece = QUEEN;
                    v.push_back(m);

                    Move m2;
                    m2.from = square;
                    m2.to = capture;
                    m2.flags = PROMOTION | CAPTURE;
                    m2.promotionPiece = ROOK;
                    v.push_back(m2);

                    Move m3;
                    m3.from = square;
                    m3.to = capture;
                    m3.flags = PROMOTION | CAPTURE;
                    m3.promotionPiece = BISHOP;
                    v.push_back(m3);

                    Move m4;
                    m4.from = square;
                    m4.to = capture;
                    m4.flags = PROMOTION | CAPTURE;
                    m4.promotionPiece = KNIGHT;
                    v.push_back(m4);

                } else {

                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = CAPTURE;
                    v.push_back(m);

                }
            } else {
                 // implement en passant logic, capture square must be empty this time
                // lets say white is in 25, black does a a two move square to 24(becomes next to white)
                //  now white can capture using en passant , white will become on square 16
                //  so basically for en passant we should combine two square and capture logic
                // en passant is possible with capture ONLY, so only flag possible is flags = EN_PASSANT | CAPTURE

                if(  pos.piece[square - 1] == PAWN &&
                    pos.color[square - 1] == BLACK &&
                    pos.enPassantSquare == capture &&
                    pos.piece[capture] == EMPTY
                ) {

                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = EN_PASSANT | CAPTURE;
                    v.push_back(m);

                }

            }

        }
        
        if(file < 7) {

            int capture = square - 7;
            if(0 <= capture && capture <= 63 && pos.color[capture] != pos.sideToMove && pos.piece[capture] != EMPTY) {

                if(0 <= capture && capture <= 7){
                     // 4 possible promotions, create 4 Move objects
                     Move m;
                     m.from = square;
                     m.to = capture;
                     m.flags = PROMOTION | CAPTURE;
                     m.promotionPiece = QUEEN;
                     v.push_back(m);
 
                     Move m2;
                     m2.from = square;
                     m2.to = capture;
                     m2.flags = PROMOTION | CAPTURE;
                     m2.promotionPiece = ROOK;
                     v.push_back(m2);
 
                     Move m3;
                     m3.from = square;
                     m3.to = capture;
                     m3.flags = PROMOTION | CAPTURE;
                     m3.promotionPiece = BISHOP;
                     v.push_back(m3);
 
                     Move m4;
                     m4.from = square;
                     m4.to = capture;
                     m4.flags = PROMOTION | CAPTURE;
                     m4.promotionPiece = KNIGHT;
                     v.push_back(m4);
                } else {

                
                Move m;
                m.from = square;
                m.to = capture;
                m.flags = CAPTURE;
                v.push_back(m);
                }

            } else {
                // implement en passant logic
                // lets say white is in 25, black does a a two move square to 24(becomes next to white)
                //  now white can capture using en passant , white will become on square 16
                //  so basically for en passant we should combine two square and capture logic
                // en passant is possible with capture ONLY, so only flag possible is flags = EN_PASSANT | CAPTURE

                if(  pos.piece[square + 1] == PAWN &&
                    pos.color[square + 1] == BLACK &&
                    pos.enPassantSquare == capture &&
                    pos.piece[capture] == EMPTY
                ) {

                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = EN_PASSANT | CAPTURE;
                    v.push_back(m);
                    
                }
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
            if(0 <= capture && capture <= 63 && pos.color[capture] != pos.sideToMove && pos.piece[capture] != EMPTY) {
                if(56 <= capture && capture <= 63){

                    // 4 possible promotions, create 4 Move objects
                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = PROMOTION | CAPTURE;
                    m.promotionPiece = QUEEN;
                    v.push_back(m);

                    Move m2;
                    m2.from = square;
                    m2.to = capture;
                    m2.flags = PROMOTION | CAPTURE;
                    m2.promotionPiece = ROOK;
                    v.push_back(m2);

                    Move m3;
                    m3.from = square;
                    m3.to = capture;
                    m3.flags = PROMOTION | CAPTURE;
                    m3.promotionPiece = BISHOP;
                    v.push_back(m3);

                    Move m4;
                    m4.from = square;
                    m4.to = capture;
                    m4.flags = PROMOTION | CAPTURE;
                    m4.promotionPiece = KNIGHT;
                    v.push_back(m4);

                } else {

                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = CAPTURE;
                    v.push_back(m);

                }
            }else {
                // implement en passant logic
                // lets say white is in 25, black does a a two move square to 24(becomes next to white)
                //  now white can capture using en passant , white will become on square 16
                //  so basically for en passant we should combine two square and capture logic
                // en passant is possible with capture ONLY, so only flag possible is flags = EN_PASSANT | CAPTURE

                if(  pos.piece[square - 1] == PAWN &&
                    pos.color[square - 1] == WHITE &&
                    pos.enPassantSquare == capture &&
                    pos.piece[capture] == EMPTY
                ) {

                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = EN_PASSANT | CAPTURE;
                    v.push_back(m);
                    
                }
            }

        }
        
        if(file < 7) {

            int capture = square + 9;
            if(0 <= capture && capture <= 63 && pos.color[capture] != pos.sideToMove && pos.piece[capture] != EMPTY) {
                if(56 <= capture && capture <= 63){

                    // 4 possible promotions, create 4 Move objects
                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = PROMOTION | CAPTURE;
                    m.promotionPiece = QUEEN;
                    v.push_back(m);

                    Move m2;
                    m2.from = square;
                    m2.to = capture;
                    m2.flags = PROMOTION | CAPTURE;
                    m2.promotionPiece = ROOK;
                    v.push_back(m2);

                    Move m3;
                    m3.from = square;
                    m3.to = capture;
                    m3.flags = PROMOTION | CAPTURE;
                    m3.promotionPiece = BISHOP;
                    v.push_back(m3);

                    Move m4;
                    m4.from = square;
                    m4.to = capture;
                    m4.flags = PROMOTION | CAPTURE;
                    m4.promotionPiece = KNIGHT;
                    v.push_back(m4);

                } else {

                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = CAPTURE;
                    v.push_back(m);

                }
            }else {
                // implement en passant logic
                // lets say white is in 25, black does a a two move square to 24(becomes next to white)
                //  now white can capture using en passant , white will become on square 16
                //  so basically for en passant we should combine two square and capture logic
                // en passant is possible with capture ONLY, so only flag possible is flags = EN_PASSANT | CAPTURE

                if(  pos.piece[square + 1] == PAWN &&
                    pos.color[square + 1] == WHITE &&
                    pos.enPassantSquare == capture &&
                    pos.piece[capture] == EMPTY
                ) {

                    Move m;
                    m.from = square;
                    m.to = capture;
                    m.flags = EN_PASSANT | CAPTURE;
                    v.push_back(m);
                    
                }
            }
        }
        
    }

    return v;
}

std::vector<Move> MoveGenerator::generateKnightMoves(Position& pos, int square) {

    // Vector that will store every legal knight move found.
    std::vector<Move> v;

    // Convert the 0-63 board index into its row.
    // Example: square 18 -> row 2.
    int row = square / 8;

    // Convert the 0-63 board index into its column/file.
    // Example: square 18 -> column 2.
    int col = square % 8;

    // Stores the 8 possible knight movement patterns.
    // Each pair represents:
    // (change in row, change in column)
    std::vector<std::pair<int, int> > arr;

    // Variables that will hold the destination row and column.
    int newRow;
    int newCol;

    // Knight movement: 2 rows, 1 column.
    arr.push_back(std::make_pair(2, 1));

    // Knight movement: 2 rows, -1 column.
    arr.push_back(std::make_pair(2, -1));

    // Knight movement: -2 rows, 1 column.
    arr.push_back(std::make_pair(-2, 1));

    // Knight movement: -2 rows, -1 column.
    arr.push_back(std::make_pair(-2, -1));

    // Knight movement: 1 row, 2 columns.
    arr.push_back(std::make_pair(1, 2));

    // Knight movement: 1 row, -2 columns.
    arr.push_back(std::make_pair(1, -2));

    // Knight movement: -1 row, 2 columns.
    arr.push_back(std::make_pair(-1, 2));

    // Knight movement: -1 row, -2 columns.
    arr.push_back(std::make_pair(-1, -2));

    // Check each of the 8 possible knight movements.
    for(auto &[first, second]: arr) {

        // Apply the row movement to the knight's current row.
        newRow = row + first;

        // Apply the column movement to the knight's current column.
        newCol = col + second;

        // Make sure the destination is still inside the 8x8 board.
        if((0 <= newRow && newRow <= 7) && (0 <= newCol && newCol <= 7)) {

            // Convert the destination row and column back into
            // the 0-63 square index used by the board.
            int destination = newRow * 8 + newCol;

            // If the destination is empty, the knight can move there.
            if(pos.piece[destination] == EMPTY) {

                // Create a normal (non-capturing) move.
                Move m;

                // Store the knight's starting square.
                m.from = square;

                // Store the knight's destination square.
                m.to = destination;

                // Add the move to the list of generated moves.
                v.push_back(m);

            } else if(pos.piece[destination] != EMPTY) {
                // If the destination contains an enemy piece,the knight can capture it.
                Move m;
                m.from = square;
                m.to = destination;
                m.flags = CAPTURE;
                v.push_back(m);
            }
        }
    }

    // Return all generated knight moves.
    return v;
}

std::vector<Move> MoveGenerator::generateBishopMoves(Position& pos, int square) {
    std::vector<Move> v;
    int row = square / 8;
    int col = square % 8;

    std::vector<std::pair<int, int> > arr;
    int newRow;
    int newCol;

    arr.push_back(std::make_pair(1, 1));
    arr.push_back(std::make_pair(1, -1));
    arr.push_back(std::make_pair(-1, 1));
    arr.push_back(std::make_pair(-1, -1));

    for(auto &[first, second]: arr) {
        newRow = row + first;
        newCol = col + second;

        while ((0 <= newRow && newRow <= 7) && (0 <= newCol && newCol <= 7)) {
                int destination = newRow * 8 + newCol;
            
                if (pos.piece[destination] == EMPTY) {
            
                    Move m;
                    m.from = square;
                    m.to = destination;
                    v.push_back(m);
            
                } else {
            
                    if (pos.color[destination] != pos.sideToMove) {
            
                        Move m;
                        m.from = square;
                        m.to = destination;
                        m.flags = CAPTURE;
                        v.push_back(m);
                    }
            
                    break;
                }
            
                newRow += first;
                newCol += second;
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
                    auto bishopMoves = generateBishopMoves(pos, i);
                    v.insert(v.end(), bishopMoves.begin(), bishopMoves.end());
                    break;
                }
                case ROOK: {
                    auto rookMoves = generateRookMoves(pos, i);
                    v.insert(v.end(), rookMoves.begin(), rookMoves.end());
                    break;
                }
                case QUEEN: {
                    auto queenMoves = generateQueenMoves(pos, i);
                    v.insert(v.end(), queenMoves.begin(), queenMoves.end());
                    break;
                }
                case KING: {
                   auto kingMoves = generateKingMoves(pos, i);
                   v.insert(v.end(), kingMoves.begin(), kingMoves.end());
                    break;
                }
            }

        }

    }

    return v;
}


