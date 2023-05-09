#include "hikaru.h"
#include "move.h"
#include "game.h"
#include "util.h"
#include <cstdlib>
#include <ctime>


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
