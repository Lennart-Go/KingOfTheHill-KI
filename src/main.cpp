#include "board.h"
#include <time.h>
#include "move.h"
#include "hikaru.h"

int main (int argc, char *argv[]) {

    int numberOfMaxDepth = 4;

    t_game *game = startGame(400);

    setFen(game->board, "r2qkbnr/pp1bpppp/2np4/1B6/3QP3/5N2/PPP2PPP/RNB1K2R");

    long double begin_sum = 0;

    for(int i=1; i<=numberOfMaxDepth; i++) {

        clock_t before = clock();
        long int *searchedMoves = static_cast<long *>(calloc(1, sizeof(long int)));
        *searchedMoves = 0;

        std::pair<move, float> move = alphaBetaHead(game, i, 10000000000, searchedMoves);

        clock_t difference = clock() - before;

        long double time = ((long double) difference) / CLOCKS_PER_SEC;

        printf("Depth: %i \t Time: %Lfs \t searched moves: %li \t \t best move: ", i, time, *searchedMoves);
        printMove(move.first);
        printf("\n");
    }

}