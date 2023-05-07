#include <cstdlib>
#include "game.h"
#include "move.h"


t_game *startGame() {
    t_game *game = (t_game *) calloc(1, sizeof(t_game));
    t_board *board = initializeBoard();

    game->board = board;
    game->turn = 0;  // White's turn
    game->latestMoveTime = 0;  // TODO: Set latestMoveTime to current time in ms
    game->moveHistory = List<t_move>();

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
    doMove(game->board, move);
    game->moveHistory.add(*move);

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
