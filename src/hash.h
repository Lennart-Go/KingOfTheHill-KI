#ifndef KINGOFTHEHILL_KI_HASH_H
#define KINGOFTHEHILL_KI_HASH_H

#include <cstdint>
#include "move.h"

#define OFFSET 6

#define KING 0
#define QUEEN 1
#define ROOK 2
#define BISHOP 3
#define KNIGHT 4
#define PAWN 5


uint64_t hash(const uint64_t* random, t_gameState *state);
uint64_t* init_hash();
int getFigureOnPos(t_board board, int pos);

#endif //KINGOFTHEHILL_KI_HASH_H
