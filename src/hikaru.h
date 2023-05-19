#include "move.h"
#include "board.h"
#include "game.h"

#ifndef KINGOFTHEHILL_KI_HIKARU_H
#define KINGOFTHEHILL_KI_HIKARU_H

#define QUEEN_VALUE 9
#define ROOK_VALUE 3
#define BISHOP_VALUE 3
#define KNIGHT_VALUE 5
#define PAWN_VALUE 1


t_move getMove(t_game *game, bool color);
t_move getMoveRandom(t_game *game, bool color);
int min(int depth, int alpha, int beta, t_game* game);
int max(int depth, int alpha, int beta, t_game* game);
t_move alphaBetaHead(t_game* game);

#endif //KINGOFTHEHILL_KI_HIKARU_H
