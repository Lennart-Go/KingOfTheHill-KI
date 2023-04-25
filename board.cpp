#include "board.h"

#include <cstdio>
#include <malloc.h>

// TODO: Set/Get?
// TODO: Print board
// TODO: Do move
// TODO: Undo move
// TODO: State information -> King moved, castled?

t_board* initializeBoard() {
    //black
    t_board* board = (t_board*) calloc(1, sizeof(t_board));
    board->king     |= (uint64_t) 1 << 4;
    board->queen    |= (uint64_t) 1 << 3;
    board->rook     |= (uint64_t) ROOK_PLACE;
    board->bishop   |= (uint64_t) BISHOP_PLACE;
    board->knight   |= (uint64_t) KNIGHT_PLACE;
    board->pawn     |= (uint64_t) PAWN_PLACE << 8;
    board->black    |= (uint64_t) COLOR_PLACE;

    //white
    board->king     |= (uint64_t) 1 << 60;
    board->queen    |= (uint64_t) 1 << 59;
    board->rook     |= (uint64_t) ROOK_PLACE << 56;
    board->bishop   |= (uint64_t) BISHOP_PLACE << 56;
    board->knight   |= (uint64_t) KNIGHT_PLACE << 56;
    board->pawn     |= (uint64_t) PAWN_PLACE << 48;
    board->white    |= (uint64_t) COLOR_PLACE << 48;

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

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; ++j) {
            printf("%c ", boardChar[i * 8 + j]);
        }
        printf("\n");
    }

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