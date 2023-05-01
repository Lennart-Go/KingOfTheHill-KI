#include <cstdint>

#ifndef KINGOFTHEHILL_KI_BOARD_H
#define KINGOFTHEHILL_KI_BOARD_H

/*
 * Board representation (Bitmaps as 1d arrays):
 * 1x White Positions (8 bytes)
 * 1x Black Positions (8 bytes)
 * 6x Figure Positions (6x8 bytes)
 * => Total 64 bytes (8 * uint64_t)
 */

typedef struct board {
    uint64_t white;
    uint64_t black;
    uint64_t king;
    uint64_t queen;
    uint64_t rook;
    uint64_t bishop;
    uint64_t knight;
    uint64_t pawn;
} t_board;

#define O_BOARD_WHITE 0
#define O_BOARD_BLACK 64
#define O_BOARD_KING 128
#define O_BOARD_QUEEN 192
#define O_BOARD_ROOK 256
#define O_BOARD_BISHOP 320
#define O_BOARD_KNIGHT 384

#endif
