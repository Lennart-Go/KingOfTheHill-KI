#include <cstdlib>
#include "game.h"
#include "move.h"
#include "hikaru.h"


t_game *startGame() {
    t_game *game = (t_game *) calloc(1, sizeof(t_game));
    t_board *board = initializeBoard();

    game->board = board;
    game->turn = 0;  // White's turn
    game->latestMoveTime = 0;  // TODO: Set latestMoveTime to current time in ms
    game->moveHistory = List<t_move>();
    game->isOver = false;

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

    game->moveHistory.add(*move);

    game->turn = !game->turn;

    // TODO: Implement end-check with end.cpp/end.h

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
        // Print current board state
        printf("Current board state (Round: %d, Turn: %d)", (game->blackMoveCounter + 1), game->turn);
        printBoard(game->board);

        // Generate next move
        t_move nextMove = getMove(game->board, game->turn);
        commitMove(game, &nextMove);

        printf("Next move: ");
        printMove(nextMove);

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

    printf("Game loop broken\n");
}
