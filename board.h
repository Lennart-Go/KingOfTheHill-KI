#include <cstdint>
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

#define UNICODE_WHITE_KING 0x2654
#define UNICODE_WHITE_QUEEN 0x2655
#define UNICODE_WHITE_ROOK 0x2656
#define UNICODE_WHITE_BISHOP 0x2657
#define UNICODE_WHITE_KNIGHT 0x2658
#define UNICODE_WHITE_PAWN 0x2659

#define UNICODE_BLACK_KING 0x265A
#define UNICODE_BLACK_QUEEN 0x265B
#define UNICODE_BLACK_ROOK 0x265C
#define UNICODE_BLACK_BISHOP 0x265D
#define UNICODE_BLACK_KNIGHT 0x265E
#define UNICODE_BLACK_PAWN 0x265F

#define UNICODE_SQUARE 0x2610

#define ROOK_PLACE 129
#define BISHOP_PLACE 36
#define KNIGHT_PLACE 66
#define PAWN_PLACE 255
#define COLOR_PLACE 65535

t_board* initializeBoard();void printBoard(t_board*);void debug_printSingleBoard(uint64_t singeBoard);