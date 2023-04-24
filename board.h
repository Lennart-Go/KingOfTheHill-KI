#ifndef KINGOFTHEHILL_KI_BOARD_H
#define KINGOFTHEHILL_KI_BOARD_H

#endif


/*
 * Board representation (Bitmaps as 1d arrays):
 * 1x White Positions (8 bytes)
 * 1x Black Positions (8 bytes)
 * 6x Figure Positions (6x8 bytes)
 * => Total 64 bytes (8 * t_uint64)
 */

typedef struct _board {
    t_uint64 white;
    t_uint64 black;
    t_uint64 king;
    t_uint64 queen;
    t_uint64 rook;
    t_uint64 bishop;
    t_uint64 knight;
    t_uint64 pawn;
} t_board;

#define O_BOARD_WHITE 0
#define O_BOARD_BLACK 64
#define O_BOARD_KING 128
#define O_BOARD_QUEEN 192
#define O_BOARD_ROOK 256
#define O_BOARD_BISHOP 320
#define O_BOARD_KNIGHT 384
