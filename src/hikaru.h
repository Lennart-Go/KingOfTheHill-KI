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


t_move getMove(t_game *game, bool color, uint64_t timePerMove);
std::pair<t_move, float> alphaBetaHead(t_game* game, int max_depth, uint64_t timePerMove);
float evaluate(const t_game *game);

#endif //KINGOFTHEHILL_KI_HIKARU_H
