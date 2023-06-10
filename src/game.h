#include "board.h"
#include "util.h"
#include <map>

#ifndef KINGOFTHEHILL_KI_GAME_H
#define KINGOFTHEHILL_KI_GAME_H

typedef struct game {
    t_board *board;

    field gameTime;
    unsigned turn:1;  // 0 for white, 1 for black
    field latestMoveTime;
    bool isOver;
    std::map<std::string,int> *positionHistory;

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

    unsigned enpassants:4;
} t_game;

t_game *startGame(field gameTime);
void play(int maxRounds, field gameTime);

#endif //KINGOFTHEHILL_KI_GAME_H
