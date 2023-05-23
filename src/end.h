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
bool isCheckmate(t_game* game, bool moving_color);
bool isStalemate(t_game* game, bool moving_color);
bool KingOfTheHill(t_board* board, bool moved_color);
bool positionTracking(t_game* game);
void positionTrackingUndo(t_game *game);
int countFigure(uint64_t singleFigure);
//bool isInsufficientMatingMaterial(t_board* board);


#endif //KINGOFTHEHILL_KI_END_H
