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


t_move_old getMove(t_game *game, bool color, field timePerMove);
std::pair<t_move_old, float> alphaBetaHead(t_game* game, int max_depth, field timePerMove);
float evaluate(const t_game *game);

#endif //KINGOFTHEHILL_KI_HIKARU_H
