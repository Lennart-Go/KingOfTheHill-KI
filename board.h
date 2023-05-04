#include <cstdint>
#include "move.h"

#ifndef KINGOFTHEHILL_KI_BOARD_H
#define KINGOFTHEHILL_KI_BOARD_H

/*
 * Board representation (Bitmaps as 1d arrays):
 * 1x White Positions (8 bytes)
 * 1x Black Positions (8 bytes)
 * 6x Figure Positions (6x8 bytes)
 * => Total 64 bytes (8 * uint64_t)
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

#define UNICODE_WHITE_KING L'\x2654'
#define UNICODE_WHITE_QUEEN L'\x2655'
#define UNICODE_WHITE_ROOK L'\x2656'
#define UNICODE_WHITE_BISHOP L'\x2657'
#define UNICODE_WHITE_KNIGHT L'\x2658'
#define UNICODE_WHITE_PAWN L'\x2659'

#define UNICODE_BLACK_KING L'\x265A'
#define UNICODE_BLACK_QUEEN L'\x265B'
#define UNICODE_BLACK_ROOK L'\x265C'
#define UNICODE_BLACK_BISHOP L'\x265D'
#define UNICODE_BLACK_KNIGHT L'\x265E'
#define UNICODE_BLACK_PAWN L'\x265F'

#define UNICODE_SQUARE L'\x2610'

#define UTF_K 75
#define UTF_Q 81
#define UTF_R 82
#define UTF_B 66
#define UTF_N 78
#define UTF_P 80
#define UTF_SLASH 47
#define UTF_BIG_SMALL_OFFSET 32
#define UTF_BIGLETTER_FIRST 65
#define UTF_BIGLETTER_LAST 90
#define UTF_NUMBER_FIRST 48
#define UTF_NUMBER_LAST 57

#define ROOK_PLACE 129
#define BISHOP_PLACE 36
#define KNIGHT_PLACE 66
#define PAWN_PLACE 255
#define COLOR_PLACE 65535

t_board* initializeBoard();
void setFen(t_board* board, char fen[]);
char* getFen(t_board* board);
void printBoard(t_board*);
char* shortenFen(char* fen);
void doMove(t_board* board, t_move* move);
void undoMove(t_board* board, t_move* move);

void debug_printSingleBoard(uint64_t singeBoard);

#endif
