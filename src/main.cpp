#include "game.h"
#include "hikaru.h"

int main() {

    if (false) { // Benchmarktests Bewertungsfunktion
        int numberOfIterations = 1000;

        game begin = t_game("r2qkbnr/pp1bpppp/2np4/1B6/3QP3/5N2/PPP2PPP/RNB1K2R", 0, 10000000);

        long double begin_sum = 0;

        for (int i = 0; i < numberOfIterations; i++) {

            clock_t before = clock();

            //evaluate2(&begin);
            clock_t difference = clock() - before;

            long double time = ((long double) difference) / CLOCKS_PER_SEC;

            begin_sum += time;
        }

        begin_sum = begin_sum / numberOfIterations;

        printf("average calculation time for opening: %Lf\n", begin_sum);

        ///////////////////////////////////////

        game middle = t_game("2r2rk1/4ppbp/3p2p1/1N6/P7/1Pn1B2P/5PP1/2R2RK1", 0, 10000000);


        long double middle_sum = 0;

        for (int i = 0; i < numberOfIterations; i++) {

            clock_t before = clock();

            //evaluate2(&middle);

            clock_t difference = clock() - before;

            long double time = ((long double) difference) / CLOCKS_PER_SEC;

            middle_sum += time;
        }

        middle_sum = middle_sum / numberOfIterations;

        printf("average calculation time for middlegame: %Lf\n", middle_sum);


        /////////////////////////////////

        game end = t_game("8/8/6k1/8/8/4P1PP/r4R2/5K2", 0, 10000000);

        long double end_sum = 0;

        for (int i = 0; i < numberOfIterations; i++) {

            clock_t before = clock();

            //evaluate2(&end);

            clock_t difference = clock() - before;

            long double time = ((long double) difference) / CLOCKS_PER_SEC;

            end_sum += time;
        }

        end_sum = end_sum / numberOfIterations;

        printf("average calculation time for endgame: %Lf\n", end_sum);

    }

    if (false) { // Benchmarktests Zuggenerierung
        int numberOfIterations = 1000;

        game begin = t_game("r2qkbnr/pp1bpppp/2np4/1B6/3QP3/5N2/PPP2PPP/RNB1K2R", 0, 10000000);
        MonteCarloTree *testTreeBegin = new MonteCarloTree(begin);


        long double begin_sum = 0;

        for (int i = 0; i < numberOfIterations; i++) {

            clock_t before = clock();

            t_gameState move = (t_gameState) monteCarlo(testTreeBegin,100,16,i).first;//generate_moves<true>(*begin.state);

            clock_t difference = clock() - before;

            long double time = ((long double) difference) / CLOCKS_PER_SEC;

            begin_sum += time;
        }

        begin_sum = begin_sum / numberOfIterations;

        printf("average calculation time for opening: %Lf\n", begin_sum);

        ///////////////////////////////////////

        game middle = t_game("2r2rk1/4ppbp/3p2p1/1N6/P7/1Pn1B2P/5PP1/2R2RK1", 0, 10000000);
        MonteCarloTree *testTreeMiddle = new MonteCarloTree(middle);


        long double middle_sum = 0;

        for (int i = 0; i < numberOfIterations; i++) {

            clock_t before = clock();

            std::vector<t_gameState> moves = generate_moves<true>(*middle.state);

            clock_t difference = clock() - before;

            long double time = ((long double) difference) / CLOCKS_PER_SEC;

            middle_sum += time;
        }

        middle_sum = middle_sum / numberOfIterations;

        printf("average calculation time for middlegame: %Lf\n", middle_sum);


        /////////////////////////////////

        game end = t_game("8/8/6k1/8/8/4P1PP/r4R2/5K2", 0, 10000000);

        long double end_sum = 0;

        for (int i = 0; i < numberOfIterations; i++) {

            clock_t before = clock();

            std::vector<t_gameState> moves = generate_moves<true>(*end.state);

            clock_t difference = clock() - before;

            long double time = ((long double) difference) / CLOCKS_PER_SEC;

            end_sum += time;
        }

        end_sum = end_sum / numberOfIterations;

        printf("average calculation time for endgame: %Lf\n", end_sum);

    }


    if (true) { // Benchmark Transposition Table
        int numberOfMaxDepth = 30;
        int stellung = 1;

        game games = game(100000000);




        if (stellung == 1) {
            games = game("r2qkbnr/pp1bpppp/2np4/1B6/3QP3/5N2/PPP2PPP/RNB1K2R", 0, 1000000000);
        } else if (stellung == 2) {
            games = game("2r2rk1/4ppbp/3p2p1/1N6/P7/1Pn1B2P/5PP1/2R2RK1", 0, 1000000000);
        } else if (stellung == 3) {
            games = game("8/8/6k1/8/8/4P1PP/r4R2/5K2", 0, 1000000000);
        }
        MonteCarloTree *testTree = new MonteCarloTree(games);

        long double begin_sum = 0;

        for (int i = 1; i <= numberOfMaxDepth; i++) {

            while (games.tableWhite._hashTable->size() > 0) {
                TableEntry entry = games.tableWhite._hashTable->begin()->second;
                games.tableWhite.removeEntry(entry);
            }
            while (games.tableBlack._hashTable->size() > 0) {
                TableEntry entry = games.tableBlack._hashTable->begin()->second;
                games.tableBlack.removeEntry(entry);
            }


            clock_t before = clock();
            long int *searchedMoves = static_cast<long *>(calloc(1, sizeof(long int)));
            *searchedMoves = 0;

            t_gameState move = (t_gameState) monteCarlo(testTree,100,16,i).first;//generate_moves<true>(*begin.state);

            clock_t difference = clock() - before;

            long double time = ((long double) difference) / CLOCKS_PER_SEC;

            printf("Depth: %i \t Time: %Lfs \t searched moves: %li \t \t best move: ", i, time, *searchedMoves);
            printMove(move);
            printf("\n");
        }
    }
}