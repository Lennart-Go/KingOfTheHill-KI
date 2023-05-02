#include "board.h"
#include "move.h"


#ifndef KINGOFTHEHILL_KI_END_H
#define KINGOFTHEHILL_KI_END_H

#endif //KINGOFTHEHILL_KI_END_H


typedef enum {
    WHITE,
    DRAW,
    BLACK,
    NOTOVER
} winner_t;

winner_t checkEnd(t_board* board, bool moved_color); //return enumerated type which party won
bool isCheckmate(t_board* board, bool moving_color);
bool isStalemate(t_board* board, bool moving_color);
bool isInsufficientMatingMaterial(t_board* board);
bool KingOfTheHill(t_board* board, bool moved_color);
