#include "board.h"
#include "util.h"

#ifndef KINGOFTHEHILL_KI_GAME_H
#define KINGOFTHEHILL_KI_GAME_H

typedef struct game {
    t_board *board;
    unsigned turn:1;  // 0 for white, 1 for black
    uint64_t latestMoveTime;
    bool isOver;

    unsigned whiteWon:1;
    unsigned whiteCanCastleShort:1;
    unsigned whiteCanCastleLong:1;
    unsigned whiteCastled:1;
    uint32_t whiteMoveCounter;
    uint32_t whiteMoveTime;  // Cumulative move time of white team in ms

    unsigned blackWon:1;
    unsigned blackCanCastleShort:1;
    unsigned blackCanCastleLong:1;
    unsigned blackCastled:1;
    uint32_t blackMoveCounter;
    uint32_t blackMoveTime;  // Cumulative move time of black team in ms
} t_game;

t_game *startGame();
void play();

#endif //KINGOFTHEHILL_KI_GAME_H
