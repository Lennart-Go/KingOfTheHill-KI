#include <cstdlib>
#include "game.h"
#include "hikaru.h"
#include "end.h"
#include "move.h"
#include "unistd.h"


t_game *startGame(uint64_t gameTime) {
    t_game *game = (t_game *) calloc(1, sizeof(t_game));
    t_board *board = initializeBoard();

    game->board = board;
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

    t_game *game = startGame(gameTime);
    // setFen(game->board, (char *)"r3k2r/8/8/8/8/8/p1r4r/R3K2R");

    printBoard(game->board);
    // sleep(4);

    uint64_t timePerMove = gameTime / 40;

    while (!game->isOver && (game->blackMoveCounter + 1) <= maxRounds) {
        /*  if (game->blackMoveCounter > 0) {
              break;
          } */

        // sleep(1);

        // Generate next move
        t_move nextMove = getMove(game, game->turn, timePerMove);
        commitMove(game, &nextMove);

        // Print next move and resulting board state
        printf("Next move: ");
        printMove(nextMove);

        printf("\nCurrent board state (Score: %.4f, Round: %d, ", evaluate(game), game->blackMoveCounter + 1);

        if (game->turn == 0) {
            printf("Turn: White)\n");
        } else {
            printf("Turn: Black)\n");
        }


        printBoard(game->board);

        // Check and announce checks
        Position kingPosition;
        if (!game->turn) {
            kingPosition = board_value_positions(game->board->white & game->board->king).at(0);
        } else {
            kingPosition = board_value_positions(game->board->black & game->board->king).at(0);
        }

        if (is_threatened(game->board, kingPosition, game->turn)) {
            printf("CHECK\n");
        }
    }

    printf("Game over!\n");
    printf("Game is over: %d\n", game->isOver);
    printf("White won: %d\n", game->whiteWon);
    printf("Black won: %d\n", game->blackWon);
    printf("Total rounds: %d\n", game->blackMoveCounter + 1);
    printf("Turn: %d\n", game->turn);

    // Free memory
    delete game->positionHistory;
    free(game);
}
