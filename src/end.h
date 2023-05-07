#include "board.h"
#include "move.h"
#include "game.h"

#ifndef KINGOFTHEHILL_KI_END_H
#define KINGOFTHEHILL_KI_END_H

typedef enum {
    NOTOVER,
    WHITE,
    BLACK,
    DRAW
} winner_t;

winner_t checkEnd(t_game *game, bool moved_color); //return enumerated type which party won
bool isCheckmate(t_board* board, bool moving_color);
bool isStalemate(t_board* board, bool moving_color);
bool isInsufficientMatingMaterial(t_board* board);
bool KingOfTheHill(t_board* board, bool moved_color);

#endif //KINGOFTHEHILL_KI_END_H
