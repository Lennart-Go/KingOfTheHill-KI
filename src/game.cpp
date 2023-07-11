#include <cstdint>
#include <cstdio>
#include <utility>
#include <chrono>

#include "board.h"
#include "game.h"
#include "hash.h"
#include "hikaru.h"


// 2 h = 2 * 60 * 60s / 40 moves
int time_limit() {
    return 2 * 60 * 60 / 40;
}

void playAlphaBeta(int maxRounds, uint64_t gameTime) {
    if (maxRounds < 0) {
        maxRounds = INT32_MAX;
    }

    // t_game game = t_game((char *)"rnbqkbnr/pppppp2/8/6P1/5Pp1/8/PPPPP3/RNBQKBNR", true, 0b1111, 8, gameTime);
    t_game game = t_game(gameTime);
    printBoard(game.state->board);


//    std::vector<t_gameState> moves = generate_moves<true>(*game.state);
//
//    for (auto move : moves) {
//        printMove(move);
//        printBoard(move.board);
//        debug_printSingleBoard(move.board.occupied);
//        printf("\n");
//    }

    // Generate next move
    t_gameState *nextMove = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
    while (!game.isOver && (game.moveCounter/2 + 1) <= maxRounds) {
        std::chrono::nanoseconds diff;
        if (game.turn) {
            // Black's turn
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            std::pair<gameState, float> result = getMoveAlphaBeta<true>(&game);
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            memcpy(nextMove, &result.first, sizeof(t_gameState));

            printf("Found move ");
            printMove(*nextMove, ' ');
            printf("with score %f for black [%fs]\n", result.second, (double )diff.count() / 1e9);
        } else {
            // White's turn
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            std::pair<gameState, float> result = getMoveAlphaBeta<false>(&game);
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            memcpy(nextMove, &result.first, sizeof(t_gameState));

            printf("Found move ");
            printMove(*nextMove, ' ');
            printf("with score %f [%fs]\n", result.second, (double )diff.count() / 1e9);
        }

        game.commitMoveTimed(*nextMove);

        // Print game state information
        printf("Current board state (Score: %.4f, Round: %d, ", evaluate(&game), game.moveCounter/2 + 1);

        if (game.turn == 0) {
            printf("Turn: White)\n");
        } else {
            printf("Turn: Black)\n");
        }

        // Check and announce checks
        bool threatened;
        if (game.turn) {
            threatened = game.board().blackKing & getThreatenedBlack(game.board());
        } else {
            threatened = game.board().whiteKing & getThreatenedWhite(game.board());
        }

        if (threatened) {
            printf("CHECK\n");
        }

        // Print board
        printBoard(game.board());


    }
    free(nextMove);

    printf("Game over!\n");
    printf("Game is over: %d\n", game.isOver);
    printf("White won: %d\n", game.whiteWon);
    printf("Black won: %d\n", game.blackWon);
    printf("Total rounds: %d\n", game.moveCounter/2 + 1);
    printf("Turn: %d\n", game.turn);

    // Free memory
    delete game.positionHistory;
    free(game.random);
    free(game.state);
}


void playMonteCarlo(int maxRounds, uint64_t gameTime) {
    if (maxRounds < 0) {
        maxRounds = INT32_MAX;
    }

    // t_game game = t_game((char *)"rnbqkbnr/pppppp2/8/6P1/5Pp1/8/PPPPP3/RNBQKBNR", true, 0b1111, 8, gameTime);
    t_game game = t_game(gameTime);
    printBoard(game.state->board);

    MonteCarloTree *initialTree = new MonteCarloTree(game);
    MonteCarloTree *currentTree = initialTree;

//    std::vector<t_gameState> moves = generate_moves<true>(*game.state);
//
//    for (auto move : moves) {
//        printMove(move);
//        printBoard(move.board);
//        debug_printSingleBoard(move.board.occupied);
//        printf("\n");
//    }

    // Generate next move
    t_gameState *nextMove = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
    while (!game.isOver && (game.moveCounter/2 + 1) <= maxRounds) {
        std::chrono::nanoseconds diff;

        // Black's turn
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        std::pair<gameState, MonteCarloTree *> result = getMoveMonteCarlo(currentTree);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        memcpy(nextMove, &result.first, sizeof(t_gameState));
        currentTree = result.second;

        printf("Found move ");
        printMove(*nextMove, ' ');
        if (game.turn) {
            printf("for black [%fs]\n", (double )diff.count() / 1e9);
        } else {
            printf("for white [%fs]\n", (double )diff.count() / 1e9);
        }

        game.commitMoveTimed(*nextMove);

        // Print game state information
        printf("Current board state (Score: %.4f, Round: %d, ", evaluate(&game), game.moveCounter/2 + 1);

        if (game.turn == 0) {
            printf("Turn: White)\n");
        } else {
            printf("Turn: Black)\n");
        }

        // Check and announce checks
        bool threatened;
        if (game.turn) {
            threatened = game.board().blackKing & getThreatenedBlack(game.board());
        } else {
            threatened = game.board().whiteKing & getThreatenedWhite(game.board());
        }

        if (threatened) {
            printf("CHECK\n");
        }

        // Print board
        printBoard(game.board());


    }
    free(nextMove);

    printf("Game over!\n");
    printf("Game is over: %d\n", game.isOver);
    printf("White won: %d\n", game.whiteWon);
    printf("Black won: %d\n", game.blackWon);
    printf("Total rounds: %d\n", game.moveCounter/2 + 1);
    printf("Turn: %d\n", game.turn);

    // Free memory
    delete game.positionHistory;
    free(game.random);
    free(game.state);
}
