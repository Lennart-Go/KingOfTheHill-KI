#include <cstdlib>
#include "game.h"
#include "hikaru.h"
#include "end.h"


t_game *startGame() {
    t_game *game = (t_game *) calloc(1, sizeof(t_game));
    t_board *board = initializeBoard();

    game->board = board;
    game->turn = 0;  // White's turn
    game->latestMoveTime = 0;  // TODO: Set latestMoveTime to current time in ms
    game->isOver = false;
    game->positionHistory = NULL;

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

    return game;
}

void commitMove(t_game *game, t_move *move) {
    if (move->origin == move->target) {
        // Failure move -> No moves generated
        game->isOver = true;
        return;
    }

    doMove(game->board, move);

    winner_t gameEnd = checkEnd(game, game->turn);
    if (gameEnd) {
        game->isOver = true;

        if (gameEnd == WHITE) {
            game->whiteWon = true;
        } else if (gameEnd == BLACK) {
            game->blackWon = true;
        }

        return;
    }

    game->turn = !game->turn;

    // TODO: Update moveTime, check if move was a castle or castles still possible
    if (!move->color) {
        // White moved
        game->whiteMoveCounter++;
    } else {
        // Black moved
        game->blackMoveCounter++;
    }
}

void play() {
    t_game *game = startGame();

    while (!game->isOver) {
//        if (game->blackMoveCounter > 50) {
//            break;
//        }

        // Generate next move
        t_move nextMove = getMove(game, game->turn);
        commitMove(game, &nextMove);

        // Print next move and resulting board state
        printf("Next move: ");
        printMove(nextMove);

        printf("Current board state (Round: %d, Turn: %d)", (game->blackMoveCounter + 1), game->turn);
        printBoard(game->board);

        // Check and announce checks
        Position kingPosition;
        if (!game->turn) {
            kingPosition = board_value_positions(game->board->white & game->board->king).get(0);
        } else {
            kingPosition = board_value_positions(game->board->black & game->board->king).get(0);
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
}
