#include "move.h"
#include "board.h"
#include "game.h"

#ifndef KINGOFTHEHILL_KI_HIKARU_H
#define KINGOFTHEHILL_KI_HIKARU_H

t_move getMove(t_game *game, bool color);
t_move getMoveRandom(t_game *game, bool color);

#endif //KINGOFTHEHILL_KI_HIKARU_H
