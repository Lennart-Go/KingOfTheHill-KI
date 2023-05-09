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

    // Initialize rng
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand((unsigned ) ts.tv_nsec);

    // Generate random number within bounds of possible moves list and return the corresponding move
    int randomMoveIndex = (int )(rand() % possibleMoves.length());
    return possibleMoves.get(randomMoveIndex);
}


t_move getMove(t_game *game, bool color) {
    return getMoveRandom(game, color);
}
