#include <cstdint>
#include <cstdio>
#include <utility>

#include "board.h"
#include "game.h"
#include "hash.h"
#include "hikaru.h"


t_gameOld *startGame(uint64_t gameTime) {
    t_gameOld *game = (t_gameOld *) calloc(1, sizeof(t_gameOld));
    t_board startBoard = initializeBoard();

    game->board = &startBoard;
    game->gameTime = gameTime;
    game->turn = 0;  // White's turn
    game->latestMoveTime = 0;  // TODO: Set latestMoveTime to current time in ms
    game->isOver = false;
    game->positionHistory = nullptr;

    game->whiteWon = false;
    game->whiteCanCastleShort = true;
    game->whiteCanCastleLong = true;
    game->whiteCastled = false;
    game->whiteMoveCounter = 0;
    game->whiteMoveTime = 0;

    game->blackWon = false;
    game->blackCanCastleShort = true;
    game->blackCanCastleLong = true;
    game->blackCastled = false;
    game->blackMoveCounter = 0;
    game->blackMoveTime = 0;

    game->enpassants = 0;

    game->random = init_hash();

    return game;
}


// 2 h = 2 * 60 * 60s / 40 moves
int time_limit() {
    return 2 * 60 * 60 / 40;
}

void play(int maxRounds, uint64_t gameTime) {
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


    uint64_t timePerMove = gameTime / 40;

    while (!game.isOver && (game.moveCounter/2 + 1) <= maxRounds) {

        // Generate next move
        t_gameState *nextMove;

        if (game.moveCounter == 7) {
            int test = 0;
        }

        if (game.turn) {
            std::pair<gameState, float> result = getMove<true>(&game, timePerMove);
            nextMove = &result.first;

            printf("Found move with score %e\n", &result.second);
        } else {
            std::pair<gameState, float> result = getMove<false>(&game, timePerMove);
            nextMove = &result.first;

            printf("Found move with score %e\n", &result.second);
        }

        game.commitMove(*nextMove);

        // Print next move and resulting board state
        printf("Next move: ");
        printMove(*nextMove);

        printf("\nCurrent board state (Score: %.4f, Round: %d, ", evaluate(&game), game.moveCounter/2 + 1);

        if (game.turn == 0) {
            printf("Turn: White)\n");
        } else {
            printf("Turn: Black)\n");
        }


        printBoard(game.board());

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
    }

    printf("Game over!\n");
    printf("Game is over: %d\n", game.isOver);
    printf("White won: %d\n", game.whiteWon);
    printf("Black won: %d\n", game.blackWon);
    printf("Total rounds: %d\n", game.moveCounter/2 + 1);
    printf("Turn: %d\n", game.turn);

    // Free memory
    delete game.positionHistory;
}


#include <algorithm>
void printMoveStack(t_game *game) {
    std::stack<t_gameState> moveStack = game->stateStack;

    std::vector<t_gameState> debugVector = std::vector<t_gameState>();
    while (!moveStack.empty( ) )
    {
        t_gameState t = moveStack.top( );
        debugVector.push_back(t);
        moveStack.pop( );
    }

    // stack, read from top down, is reversed relative to its creation (from bot to top)
    for (int i = (int )debugVector.size()-1; i >= 0; i--) {
        t_gameState currentState = debugVector.at(i);

        printMove(currentState);
        game->stateStack.push(currentState);
    }
}
