#include "../Chess/Types.h"

const int CAPTURE = 1;
const int PROMOTION = 2;
const int EN_PASSANT = 4;
const int CASTLING = 8;

struct Move {
    int from;
    int to;
    int flags = 0;
    int promotionPiece = EMPTY; // only if flag is promotion
};