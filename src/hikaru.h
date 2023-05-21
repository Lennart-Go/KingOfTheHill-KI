#include "move.h"
#include "board.h"
#include "game.h"

#ifndef KINGOFTHEHILL_KI_HIKARU_H
#define KINGOFTHEHILL_KI_HIKARU_H

#define QUEEN_VALUE 9
#define ROOK_VALUE 5
#define BISHOP_VALUE 3
#define KNIGHT_VALUE 3
#define PAWN_VALUE 1


t_move getMove(t_game *game, bool color);
t_move alphaBetaHead(t_game* game, int max_depth);

#endif //KINGOFTHEHILL_KI_HIKARU_H
