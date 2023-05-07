#include "board.h"
#include "move.h"
#include <malloc.h>

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

void setFen(t_board* board, char fen[]) {

    board->king = 0;
    board->queen = 0;
    board->rook  = 0;
    board->bishop = 0;
    board->knight = 0;
    board->pawn = 0;
    board->white = 0;
    board->black = 0;

    int j = 0;
    for (int i = 0; i < strlen(fen); i++) {
        char currChar = fen[i];
        if (currChar >= UTF_NUMBER_FIRST && currChar <= UTF_NUMBER_LAST) {
            j += (currChar - UTF_NUMBER_FIRST);
            continue;
        }
        else if (currChar == UTF_SLASH) continue;
        else if (currChar == UTF_K || currChar == UTF_K + UTF_BIG_SMALL_OFFSET) board->king |= (uint64_t) 1 << j;
        else if (currChar == UTF_Q || currChar == UTF_Q + UTF_BIG_SMALL_OFFSET) board->queen |= (uint64_t) 1 << j;
        else if (currChar == UTF_R || currChar == UTF_R + UTF_BIG_SMALL_OFFSET) board->rook |= (uint64_t) 1 << j;
        else if (currChar == UTF_B || currChar == UTF_B + UTF_BIG_SMALL_OFFSET) board->bishop |= (uint64_t) 1 << j;
        else if (currChar == UTF_N || currChar == UTF_N + UTF_BIG_SMALL_OFFSET) board->knight |= (uint64_t) 1 << j;
        else if (currChar == UTF_P || currChar == UTF_P + UTF_BIG_SMALL_OFFSET) board->pawn |= (uint64_t) 1 << j;

        if (currChar >= UTF_BIGLETTER_FIRST && currChar <= UTF_BIGLETTER_LAST) board->white |= (uint64_t) 1 << j;
        else if (currChar >= UTF_BIGLETTER_FIRST + UTF_BIG_SMALL_OFFSET && currChar < UTF_BIGLETTER_LAST + UTF_BIG_SMALL_OFFSET) board->black |= (uint64_t) 1 << j;

        j++;
    }
}

char* getFen(t_board* board) {
    char* fen = (char*)(calloc(72, sizeof(char)));
    char slash[2] = {UTF_SLASH, '\0'};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            uint64_t bitMask = ((uint64_t) 1 << (i*8 + j));
            int currChar;
            if ((board->king & bitMask) != 0) currChar = UTF_K;
            else if ((board->queen & bitMask) != 0) currChar = UTF_Q;
            else if ((board->rook & bitMask) != 0) currChar = UTF_R;
            else if ((board->bishop & bitMask) != 0) currChar = UTF_B;
            else if ((board->knight & bitMask) != 0) currChar = UTF_N;
            else if ((board->pawn & bitMask) != 0) currChar = UTF_P;
            else currChar = UTF_NUMBER_FIRST + 1;

            if ((board->black & bitMask) != 0) currChar += UTF_BIG_SMALL_OFFSET;
            char currCharchar[2] = {static_cast<char>(currChar), '\0'};
            strcat(fen, currCharchar);
        }
        strcat(fen, slash);
    }
    return shortenFen(fen);
}

char* shortenFen(char* fen) {
    char* newFen = (char*)(calloc(72, sizeof(char)));
    int i = 0;
    int j = 0;
    int slashes = 0;
    while (fen[i] != '\0') {
        char currChar = fen[i];
        if (currChar >= UTF_BIGLETTER_FIRST &&
            currChar <= UTF_BIGLETTER_LAST) {
            char currCharchar[2] = {(char)(currChar), '\0'};
            strcat(newFen, (char*) currCharchar);
            i++;
            j++;
            continue;
        }
        else if (currChar >= UTF_BIGLETTER_FIRST + UTF_BIG_SMALL_OFFSET &&
                 currChar <= UTF_BIGLETTER_LAST + UTF_BIG_SMALL_OFFSET) {
            char currCharchar[2] = {(char)(currChar), '\0'};
            strcat(newFen, (char*) currCharchar);
            i++;
            j++;
            continue;
        }
        else if (currChar == UTF_SLASH) {
            if (slashes < 7) {
                char currCharchar[2] = {(char)(currChar), '\0'};
                strcat(newFen, (char*) currCharchar);
                i++;
                j++;
                slashes++;
                continue;
            }
            else {
                char currCharchar[1] = {'\0'};
                strcat(newFen, currCharchar);
                break;
            }
        }
        else if (currChar >= UTF_NUMBER_FIRST &&
                 currChar <= UTF_NUMBER_LAST) {
            int skips = 0;
            while (fen[i] == '1') {
                skips += 1;
                i++;
            }
            skips += UTF_NUMBER_FIRST;
            char currCharchar[2] = {(char)(skips), '\0'};
            strcat(newFen, currCharchar);
        }
    }
    free(fen);
    return newFen;
}

void printBoard(t_board* board) {
    wchar_t boardChar[64] = {};

    uint64_t whiteKing = board->king & board->white;
    uint64_t blackKing = board->king & board->black;

    uint64_t whiteQueen = board->queen & board->white;
    uint64_t blackQueen = board->queen & board->black;

    uint64_t whiteRook = board->rook & board->white;
    uint64_t blackRook = board->rook & board->black;

    uint64_t whiteBishop = board->bishop & board->white;
    uint64_t blackBishop = board->bishop & board->black;

    uint64_t whiteKnight = board->knight & board->white;
    uint64_t blackKnight = board->knight & board->black;

    uint64_t whitePawn = board->pawn & board->white;
    uint64_t blackPawn = board->pawn & board->black;

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
    
    // int mode = _setmode(STDOUT_FILENO, _O_U16TEXT);
    // 
    // wprintf(L"\n");
    // for (int i = 0; i < 8; i++) {
    //     wprintf(L"|");
    //     for (int j = 0; j < 8; ++j) {
    //         wprintf(L"%lc ", boardChar[i * 8 + j]);
    //     }
    //     wprintf(L"|\n");
    // }
    // wprintf(L"\n");
    // 
    // _setmode(STDOUT_FILENO, mode);
    
    setlocale(LC_ALL, "en_US.UTF-8");

    printf("\n");
    for (int i = 0; i < 8; i++) {
        printf("|");
        for (int j = 0; j < 8; ++j) {
            printf("%lc ", boardChar[i * 8 + j]);
        }
        printf("|\n");
    }
    printf("\n");

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