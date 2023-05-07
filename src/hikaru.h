#include "move.h"
#include "board.h"

#ifndef KINGOFTHEHILL_KI_HIKARU_H
#define KINGOFTHEHILL_KI_HIKARU_H

t_move getMove(t_board *board, bool color);
t_move getMoveRandom(t_board *board, bool color);

#endif //KINGOFTHEHILL_KI_HIKARU_H
