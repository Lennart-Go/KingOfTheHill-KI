#include "board.h"
#include "move.h"
#include <cstdio>
#include <malloc.h>
#include <fcntl.h>

// TODO: Set/Get?
// TODO: Print board
// TODO: Do move
// TODO: Undo move
// TODO: State information -> King moved, castled?

t_board* initializeBoard() {

    t_board* board = (t_board*) calloc(1, sizeof(t_board));

    board->king     |= (uint64_t) 1 << 4;
    board->queen    |= (uint64_t) 1 << 3;
    board->rook     |= (uint64_t) ROOK_PLACE;
    board->bishop   |= (uint64_t) BISHOP_PLACE;
    board->knight   |= (uint64_t) KNIGHT_PLACE;
    board->pawn     |= (uint64_t) PAWN_PLACE << 8;
    board->white    |= (uint64_t) COLOR_PLACE << 48;

    board->king     |= (uint64_t) 1 << 60;
    board->queen    |= (uint64_t) 1 << 59;
    board->rook     |= (uint64_t) ROOK_PLACE << 56;
    board->bishop   |= (uint64_t) BISHOP_PLACE << 56;
    board->knight   |= (uint64_t) KNIGHT_PLACE << 56;
    board->pawn     |= (uint64_t) PAWN_PLACE << 48;
    board->black    |= (uint64_t) COLOR_PLACE;

    return board;
}

void printBoard(t_board* board) {
    wchar_t boardChar[64] = {' '};

    int64_t whiteKing = board->king & board->white;
    int64_t blackKing = board->king & board->black;

    int64_t whiteQueen = board->queen & board->white;
    int64_t blackQueen = board->queen & board->black;

    int64_t whiteRook = board->rook & board->white;
    int64_t blackRook = board->rook & board->black;

    int64_t whiteBishop = board->bishop & board->white;
    int64_t blackBishop = board->bishop & board->black;

    int64_t whiteKnight = board->knight & board->white;
    int64_t blackKnight = board->knight & board->black;

    int64_t whitePawn = board->pawn & board->white;
    int64_t blackPawn = board->pawn & board->black;

    //KING
    for (int i = 0; i < 64; i++) {
        if ((whiteKing & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_KING;
        }
    }

    for (int i = 0; i < 64; i++) {
        if ((blackKing & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_KING;
        }
    }

    //QUEEN
    for (int i = 0; i < 64; i++) {
        if ((whiteQueen & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_QUEEN;
        }
    }

    for (int i = 0; i < 64; i++) {
        if ((blackQueen & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_QUEEN;
        }
    }

    //ROOK
    for (int i = 0; i < 64; i++) {
        if ((whiteRook & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_ROOK;
        }
    }

    for (int i = 0; i < 64; i++) {
        if ((blackRook & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_ROOK;
        }
    }

    //BISHOP
    for (int i = 0; i < 64; i++) {
        if ((whiteBishop & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_BISHOP;
        }
    }

    for (int i = 0; i < 64; i++) {
        if ((blackBishop & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_BISHOP;
        }
    }

    //KNIGHT
    for (int i = 0; i < 64; i++) {
        if ((whiteKnight & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_KNIGHT;
        }
    }

    for (int i = 0; i < 64; i++) {
        if ((blackKnight & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_KNIGHT;
        }
    }

    //PAWN
    for (int i = 0; i < 64; i++) {
        if ((whitePawn & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_PAWN;
        }
    }

    for (int i = 0; i < 64; i++) {
        if ((blackPawn & ((uint64_t) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_PAWN;
        }
    }

    //SQUARE
    for (int i = 0; i < 64; i++) {
        if (boardChar[i] == L'\000') {
            boardChar[i] = UNICODE_SQUARE;
        }
    }
    
    int mode = _setmode(_fileno(stdout), _O_U16TEXT);

    wprintf(L"\n");
    for (int i = 0; i < 8; i++) {
        wprintf(L"|");
        for (int j = 0; j < 8; ++j) {
            wprintf(L"%lc ", boardChar[i * 8 + j]);
        }
        wprintf(L"|\n");
    }
    wprintf(L"\n");

    _setmode(_fileno(stdout), mode);

}

void doMove(t_board* board, t_move* move) {

    //generate bitmask for fields
    uint64_t bitOrigin = (uint64_t) 1 << move->origin;
    uint64_t bitTarget = (uint64_t) 1 << move->target;

    //set taken figure
    if ((board->queen & bitTarget) != 0) move->taken_figure = 1;
    else if ((board->rook & bitTarget) != 0) move->taken_figure = 2;
    else if ((board->bishop & bitTarget) != 0) move->taken_figure = 3;
    else if ((board->knight & bitTarget) != 0) move->taken_figure = 4;
    else if ((board->pawn & bitTarget) != 0) move->taken_figure = 5;
    else move->taken_figure = 0;

    //clear targetfield
    board->king &= ~bitTarget;
    board->queen &= ~bitTarget;
    board->rook &= ~bitTarget;
    board->bishop &= ~bitTarget;
    board->knight &= ~bitTarget;
    board->pawn &= ~bitTarget;
    board->white &= ~bitTarget;
    board->black &= ~bitTarget;

    //get originfigure and move it
    if ((board->king & bitOrigin) != 0) {
        board->king&= ~bitOrigin;
        board->king|= bitTarget;
    }
    else if ((board->queen & bitOrigin) != 0) {
        board->queen&= ~bitOrigin;
        board->queen|= bitTarget;
    }
    else if ((board->rook & bitOrigin) != 0) {
        board->rook&= ~bitOrigin;
        board->rook|= bitTarget;
    }
    else if ((board->bishop & bitOrigin) != 0) {
        board->bishop&= ~bitOrigin;
        board->bishop|= bitTarget;
    }
    else if ((board->knight & bitOrigin) != 0) {
        board->knight&= ~bitOrigin;
        board->knight|= bitTarget;
    }
    else if ((board->pawn & bitOrigin) != 0) {
        board->pawn &= ~bitOrigin;
        board->pawn |= bitTarget;
    }

    //clear originfield and set move color
    if ((board->black & bitOrigin) != 0) {
        board->black &= ~bitOrigin;
        move->color = 1;
    }
    else if ((board->white & bitOrigin) != 0) {
        board->white &= ~bitOrigin;
        move->color = 0;
    }

    //set color of targefield to move color
    if (move->color == 0) {
        board->black &= ~bitTarget;
        board->white |= bitTarget;
    }
    else if (move->color == 1) {
        board->white &= ~bitTarget;
        board->black |= bitTarget;
    }
}

void undoMove(t_board* board, t_move* move) {

    //generate bitmask for fields
    uint64_t bitOrigin = (uint64_t) 1 << move->origin;
    uint64_t bitTarget = (uint64_t) 1 << move->target;

    //get targetfigure and move it
    if ((board->king & bitTarget) != 0) {
        board->king&= ~bitTarget;
        board->king|= bitOrigin;
    }

    else if ((board->queen & bitTarget) != 0) {
        board->queen&= ~bitTarget;
        board->queen|= bitOrigin;
    }

    else if ((board->rook & bitTarget) != 0) {
        board->rook&= ~bitTarget;
        board->rook|= bitOrigin;
    }

    else if ((board->bishop & bitTarget) != 0) {
        board->bishop&= ~bitTarget;
        board->bishop|= bitOrigin;
    }

    else if ((board->knight & bitTarget) != 0) {
        board->knight&= ~bitTarget;
        board->knight|= bitOrigin;
    }

    else if ((board->pawn & bitTarget) != 0) {
        board->pawn &= ~bitTarget;
        board->pawn |= bitOrigin;
    }

    //clear targetfield
    board->black    &= ~bitTarget;
    board->white    &= ~bitTarget;


    board->black &= ~bitTarget;
    board->white |= bitTarget;

    //set color of originfield
    if (move->color == 0) {
        board->black &= ~bitOrigin;
        board->white |= bitOrigin;

        board->black |= bitTarget;

    }
    else if (move->color == 1) {
        board->white &= ~bitOrigin;
        board->black |= bitOrigin;

        board->white |= bitTarget;
    }

    //place takenfigure
    if (move->taken_figure == 0) {}
    else if (move->taken_figure == 1) board->queen |= bitTarget;
    else if (move->taken_figure == 2) board->rook |= bitTarget;
    else if (move->taken_figure == 3) board->bishop |= bitTarget;
    else if (move->taken_figure == 4) board->knight |= bitTarget;
    else if (move->taken_figure == 5) board->pawn |= bitTarget;

}

void debug_printSingleBoard(uint64_t singleBoard) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (((singleBoard) & ((uint64_t)1 << (i * 8 + j) )) >> (i * 8 + j) == 1) {
                printf("1 ");
            }
            else {
                printf("0 ");
            }
        }
        printf("\n");
    }
}