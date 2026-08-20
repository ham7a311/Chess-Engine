#include "../Chess/Types.h"

const int Capture = 1;
const int Promotion = 2;
const int EN_Passant = 4;
const int Castling = 8;

struct Move {
    int from;
    int to;
    int flags = 0;
    int promotionPiece = EMPTY; // only if flag is promotion
};