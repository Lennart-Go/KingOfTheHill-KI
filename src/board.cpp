#include "board.h"
#include "move.h"
#include <malloc.h>

t_board setFen(char *fen) {

    field king = 0;
    field queen = 0;
    field rook  = 0;
    field bishop = 0;
    field knight = 0;
    field pawn = 0;
    field white = 0;
    field black = 0;

    int j = 0;
    for (int i = 0; i < strlen(fen); i++) {
        char currChar = fen[i];
        if (currChar >= UTF_NUMBER_FIRST && currChar <= UTF_NUMBER_LAST) {
            j += (currChar - UTF_NUMBER_FIRST);
            continue;
        }
        else if (currChar == UTF_SLASH) continue;
        else if (currChar == UTF_K || currChar == UTF_K + UTF_BIG_SMALL_OFFSET) king |= (field) 1 << j;
        else if (currChar == UTF_Q || currChar == UTF_Q + UTF_BIG_SMALL_OFFSET) queen |= (field) 1 << j;
        else if (currChar == UTF_R || currChar == UTF_R + UTF_BIG_SMALL_OFFSET) rook |= (field) 1 << j;
        else if (currChar == UTF_B || currChar == UTF_B + UTF_BIG_SMALL_OFFSET) bishop |= (field) 1 << j;
        else if (currChar == UTF_N || currChar == UTF_N + UTF_BIG_SMALL_OFFSET) knight |= (field) 1 << j;
        else if (currChar == UTF_P || currChar == UTF_P + UTF_BIG_SMALL_OFFSET) pawn |= (field) 1 << j;

        if (currChar >= UTF_BIGLETTER_FIRST && currChar <= UTF_BIGLETTER_LAST) white |= (field) 1 << j;
        else if (currChar >= UTF_BIGLETTER_FIRST + UTF_BIG_SMALL_OFFSET && currChar < UTF_BIGLETTER_LAST + UTF_BIG_SMALL_OFFSET) black |= (field) 1 << j;

        j++;
    }

    return board(white & king, white & queen, white & rook, white & bishop, white & knight, white & pawn,
                 black & king, black & queen, black & rook, black & bishop, black & knight, black & pawn);
}

t_board initializeBoard() {
    return setFen((char *) "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}



char* getFen(t_board *board) {
    char* fen = (char*)(calloc(73, sizeof(char)));
    char slash[2] = {UTF_SLASH, '\0'};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            field bitMask = ((field) 1 << (i*8 + j));
            int currChar;
            if ((board->whiteKing & bitMask) != 0) currChar = UTF_K;
            else if ((board->whiteQueen & bitMask) != 0) currChar = UTF_Q;
            else if ((board->whiteRook & bitMask) != 0) currChar = UTF_R;
            else if ((board->whiteBishop & bitMask) != 0) currChar = UTF_B;
            else if ((board->whiteKnight & bitMask) != 0) currChar = UTF_N;
            else if ((board->whitePawn & bitMask) != 0) currChar = UTF_P;
            else if ((board->blackKing & bitMask) != 0) currChar = UTF_k;
            else if ((board->blackQueen & bitMask) != 0) currChar = UTF_q;
            else if ((board->blackRook & bitMask) != 0) currChar = UTF_r;
            else if ((board->blackBishop & bitMask) != 0) currChar = UTF_b;
            else if ((board->blackKnight & bitMask) != 0) currChar = UTF_n;
            else if ((board->blackPawn & bitMask) != 0) currChar = UTF_p;
            else currChar = UTF_NUMBER_FIRST + 1;

            char currCharchar[2] = {static_cast<char>(currChar), '\0'};
            strcat(fen, currCharchar);
        }
        strcat(fen, slash);
    }
    return shortenFen(fen);
}

char* shortenFen(char* fen) {
    char* newFen = (char*)(calloc(73, sizeof(char)));
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

void printBoard(t_board *board) {
    wchar_t boardChar[64] = {};

    //KING
    for (int i = 0; i < 64; i++) {
        if ((board->whiteKing & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_KING;
        }
    }
    for (int i = 0; i < 64; i++) {
        if ((board->blackKing & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_KING;
        }
    }

    //QUEEN
    for (int i = 0; i < 64; i++) {
        if ((board->whiteQueen & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_QUEEN;
        }
    }
    for (int i = 0; i < 64; i++) {
        if ((board->blackQueen & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_QUEEN;
        }
    }

    //ROOK
    for (int i = 0; i < 64; i++) {
        if ((board->whiteRook & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_ROOK;
        }
    }
    for (int i = 0; i < 64; i++) {
        if ((board->blackRook & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_ROOK;
        }
    }

    //BISHOP
    for (int i = 0; i < 64; i++) {
        if ((board->whiteBishop & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_BISHOP;
        }
    }
    for (int i = 0; i < 64; i++) {
        if ((board->blackBishop & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_BISHOP;
        }
    }

    //KNIGHT
    for (int i = 0; i < 64; i++) {
        if ((board->whiteKnight & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_KNIGHT;
        }
    }
    for (int i = 0; i < 64; i++) {
        if ((board->blackKnight & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_KNIGHT;
        }
    }

    //PAWN
    for (int i = 0; i < 64; i++) {
        if ((board->whitePawn & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_WHITE_PAWN;
        }
    }
    for (int i = 0; i < 64; i++) {
        if ((board->blackPawn & ((field) 1 << i)) >> i) {
            boardChar[i] = UNICODE_BLACK_PAWN;
        }
    }

    //SQUARE
    for (wchar_t & i : boardChar) {
        if (i == L'\000') {
            i = UNICODE_SQUARE;
        }
    }
    
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

void debug_printSingleBoard(field singleBoard) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (((singleBoard) & ((field)1 << (i * 8 + j) )) >> (i * 8 + j) == 1) {
                printf("1 ");
            }
            else {
                printf("0 ");
            }
        }
        printf("\n");
    }
    printf("\n");
}