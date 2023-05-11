#include "hikaru.h"
#include "move.h"
#include "game.h"
#include "util.h"
#include <cstdlib>
#include <ctime>
#include "hikaru.h"


t_move getMoveRandom(t_game *game, bool color) {
    List<t_move> possibleMoves = generate_moves(game, color);

    if (possibleMoves.empty()) {
        t_move failureMove = { 0, 0, 0, 0 };
        return failureMove;
    }

    //printf("Found %zu moves!\n", possibleMoves.length());
//    for (int i = 0; i < possibleMoves.length(); ++i) {
//        printMove(possibleMoves.get(i));
//    }

    // Initialize rng
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand((unsigned ) ts.tv_nsec);

    // Generate random number within bounds of possible moves list and return the corresponding move
    int randomMoveIndex = (int )(rand() % possibleMoves.length());
    return possibleMoves.get(randomMoveIndex);
}

int primitive_evaluation_function(t_move *move) {
    return 1;
}


t_move getMove(t_game *game, bool color) {
    t_move best_move = getMoveRandom(game, color);
    primitive_evaluation_function(&best_move);

    return best_move;
}

t_move alphaBetaHead(t_game* game) {
    int depth = 3;
    if (game->turn) {
        max(depth, INT32_MIN, INT32_MAX, game);
    }

}

int value(t_game* game) {
    return 1;
}

int max(int depth, int alpha, int beta, t_game* game) {

    int maxScore = INT32_MIN;
    List<t_move> moves = generate_moves(game, false);
    if ((depth == 0) | game->isOver | moves.size() == 0) {
        return value(game);
    }
    t_move bestMove = moves.get(0);
    while (moves.length() != 0) {
        t_move currMove = moves.pop();
        doMove(game->board, &currMove);
        int score = min(depth - 1, alpha, beta, game);
        undoMove(game->board, &currMove);
        if (score > maxScore) {
            maxScore = score;
            bestMove = currMove;
        }
    }
    return maxScore;
}

int min(int depth, int alpha, int beta, t_game* game) {

    int minScore = INT32_MAX;
    List<t_move> moves = generate_moves(game, false);
    if ((depth == 0) | game->isOver | moves.size() == 0) {
        return value(game);
    }
    t_move bestMove = moves.get(0);
    while (moves.length() != 0) {
        t_move currMove = moves.pop();
        doMove(game->board, &currMove);
        int score = max(depth - 1, alpha, beta, game);
        undoMove(game->board, &currMove);
        if (score > minScore) {
            minScore = score;
            bestMove = currMove;
        }
    }
    return minScore;
}

