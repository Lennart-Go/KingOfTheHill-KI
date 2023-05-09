#include "board.h"
#include <time.h>
#include "move.h"
#include "game.h"

int main (int argc, char *argv[]) {

    play();

 /*   int numberOfIterations = 1000;

    t_board *begin = initializeBoard();

    setFen(begin, "r2qkbnr/pp1bpppp/2np4/1B6/3QP3/5N2/PPP2PPP/RNB1K2R");

    long double begin_sum = 0;

    for(int i=0; i<numberOfIterations; i++) {

        clock_t before = clock();

        generate_moves(begin, 0);

        clock_t difference = clock() - before;

        long double time = ((long double) difference) / CLOCKS_PER_SEC;

        begin_sum += time;
    }

    begin_sum = begin_sum / numberOfIterations;

    printf("average calculation time for opening: %Lf\n", begin_sum);

    ///////////////////////////////////////

    t_board *middle = initializeBoard();

    setFen(middle, "2r2rk1/4ppbp/3p2p1/1N6/P7/1Pn1B2P/5PP1/2R2RK1");

    long double middle_sum = 0;

    for(int i=0; i<numberOfIterations; i++) {

        clock_t before = clock();

        generate_moves(begin, 0);

        clock_t difference = clock() - before;

        long double time = ((long double) difference) / CLOCKS_PER_SEC;

        middle_sum += time;
    }

    middle_sum = middle_sum / numberOfIterations;

    printf("average calculation time for middlegame: %Lf\n", middle_sum);


    /////////////////////////////////

    t_board *end = initializeBoard();

    setFen(end, "8/8/6k1/8/8/4P1PP/r4R2/5K2");

    long double end_sum = 0;

    for(int i=0; i<numberOfIterations; i++) {

        clock_t before = clock();

        generate_moves(begin, 0);

        clock_t difference = clock() - before;

        long double time = ((long double) difference) / CLOCKS_PER_SEC;

        end_sum += time;
    }

    end_sum = end_sum / numberOfIterations;

    printf("average calculation time for endgame: %Lf\n", end_sum); */


}