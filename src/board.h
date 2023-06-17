#ifndef KINGOFTHEHILL_KI_BOARD_H
#define KINGOFTHEHILL_KI_BOARD_H

#include <cstdint>

#include "util.h"

/*
 * Board representation (Bitmaps as 1d arrays):
 * 1x White Positions (8 bytes)
 * 1x Black Positions (8 bytes)
 * 6x Figure Positions (6x8 bytes)
 * => Total 64 bytes (8 * field)
 */

typedef struct board {
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

    board(
            field wk, field wq, field wr, field wb, field wn, field wp,
            field bk, field bq, field br, field bb, field bn, field bp) :
            whiteKing(wk), whiteQueen(wq), whiteRook(wr), whiteBishop(wb), whiteKnight(wn), whitePawn(wp),
            blackKing(bk), blackQueen(bq), blackRook(br), blackBishop(bb), blackKnight(bn), blackPawn(bp),
            white(wk | wq | wr | wb | wn | wp),
            black(bk | bq | br | bb | bn | bp),
            occupied(white | black) {}

//    board(const board &other) :
//            whiteKing(other.whiteKing), whiteQueen(other.whiteQueen), whiteRook(other.whiteRook), whiteBishop(other.whiteBishop), whiteKnight(other.whiteKnight), whitePawn(other.whitePawn),
//            blackKing(other.blackKing), blackQueen(other.blackQueen), blackRook(other.blackRook), blackBishop(other.blackBishop), blackKnight(other.blackKnight), blackPawn(other.blackPawn),
//            white(other.white),
//            black(other.black),
//            occupied(other.occupied),
//            king(other.king), queen(other.queen), rook(other.rook), bishop(other.bishop), knight(other.knight), pawn(other.pawn) {}


    board(const board &other) = default;
//    }
} t_board;


enum class piece {
    king,
    queen,
    rook,
    bishop,
    knight,
    pawn,
    none
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

void printBoard(const t_board *board);
void printBoard(t_board board);

char *shortenFen(char *fen);

void debug_printSingleBoard(field singeBoard);

#endif
