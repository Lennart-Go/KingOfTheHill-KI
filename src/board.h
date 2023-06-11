#include <cstdint>

#ifndef KINGOFTHEHILL_KI_BOARD_H
#define KINGOFTHEHILL_KI_BOARD_H

#include "util.h"

/*
 * Board representation (Bitmaps as 1d arrays):
 * 1x White Positions (8 bytes)
 * 1x Black Positions (8 bytes)
 * 6x Figure Positions (6x8 bytes)
 * => Total 64 bytes (8 * field)
 */

typedef struct board {
    field king;
    field queen;
    field rook;
    field bishop;
    field knight;
    field pawn;

    const field whiteKing;
    const field whiteQueen;
    const field whiteRook;
    const field whiteBishop;
    const field whiteKnight;
    const field whitePawn;

    const field blackKing;
    const field blackQueen;
    const field blackRook;
    const field blackBishop;
    const field blackKnight;
    const field blackPawn;

    field white;
    field black;
    const field occupied;

    constexpr board(
            field wk, field wq, field wr, field wb, field wn, field wp,
            field bk, field bq, field br, field bb, field bn, field bp) :
            whiteKing(wk), whiteQueen(wq), whiteRook(wr), whiteBishop(wb), whiteKnight(wn), whitePawn(wp),
            blackKing(bk), blackQueen(bq), blackRook(br), blackBishop(bb), blackKnight(bn), blackPawn(bp),
            white(wk | wq | wr | wb | wn | wp),
            black(bk | bq | br | bb | bn | bp),
            occupied(white | black),
            king(wk | bk), queen(wq | bq), rook(wr | br), bishop(wb | bb), knight(wn | bn), pawn(wp | bp) {}


//    constexpr board& operator=(board const &other) {
//        return board(other.whiteKing, other.whiteQueen, other.whiteRook, other.whiteBishop, other.whiteKnight, other.whitePawn,
//                     other.blackKing, other.blackQueen, other.blackRook, other.blackBishop, other.blackKnight, other.blackPawn);
//    }
} t_board;


enum class piece {
    none,
    king,
    queen,
    rook,
    bishop,
    knight,
    pawn
};


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

#define UTF_SLASH 47
#define UTF_BIG_SMALL_OFFSET 32
#define UTF_BIGLETTER_FIRST 65
#define UTF_BIGLETTER_LAST 90
#define UTF_NUMBER_FIRST 48
#define UTF_NUMBER_LAST 57
#define UTF_K 75
#define UTF_Q 81
#define UTF_R 82
#define UTF_B 66
#define UTF_N 78
#define UTF_P 80
#define UTF_k (UTF_K + UTF_BIG_SMALL_OFFSET)
#define UTF_q (UTF_Q + UTF_BIG_SMALL_OFFSET)
#define UTF_r (UTF_R + UTF_BIG_SMALL_OFFSET)
#define UTF_b (UTF_B + UTF_BIG_SMALL_OFFSET)
#define UTF_n (UTF_N + UTF_BIG_SMALL_OFFSET)
#define UTF_p (UTF_P + UTF_BIG_SMALL_OFFSET)

#define ROOK_PLACE 129
#define BISHOP_PLACE 36
#define KNIGHT_PLACE 66
#define PAWN_PLACE 255
#define COLOR_PLACE 65535

t_board initializeBoard();

t_board setFen(char fen[]);

char *getFen(t_board *board);

void printBoard(t_board *);

char *shortenFen(char *fen);

void debug_printSingleBoard(field singeBoard);

#endif
