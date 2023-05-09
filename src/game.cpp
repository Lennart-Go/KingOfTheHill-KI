#include <cstdlib>
#include "game.h"
#include "hikaru.h"
#include "end.h"
#include "move.h"
#include "unistd.h"


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

    game->enpassants = 0;

    return game;
}

// 2 h = 2 * 60 * 60s / 40 moves
int time_limit() {
    return 2*60*60/40;
}

void commitMove(t_game *game, t_move *move) {
    if (move->origin == move->target) {
        // Failure move -> No moves generated
        game->isOver = true;
        return;
    }

    doMove(game->board, move);

    game->enpassants = 0;

    // Handle double-forward pawn move
    if (is_double_pawn_move(move)) {
        //printf("DOUBLE PAWN MOVE!\n");
        // Set according bit if the move was a double-forward pawn move
        game->enpassants |= 1 << position_from_shift(move->origin).x;
    } else if (is_castle(game->board, move)) {
        //printf("CASTLE!\n");
    } else if (is_enpassant(game->board, move)) {
        //printf("EN-PASSENT!\n");
    } else if (move->promoted) {
        //printf("PROMOTION!\n");
    }

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

        // Update castling flags
        if (!game->whiteCastled && (game->whiteCanCastleShort || game->whiteCanCastleLong)) {
            if (board_value_from_shift(game->board->king, move->target)) {
                // King moved
                game->whiteCanCastleShort = false;
                game->whiteCanCastleLong = false;
            } else if (board_value_from_shift(game->board->rook, move->target) && position_from_shift(move->target).x == 7) {
                // Short rook moved
                game->whiteCanCastleShort = false;
            } else if (board_value_from_shift(game->board->rook, move->target) && position_from_shift(move->target).x == 0) {
                game->whiteCanCastleLong = false;
            }
            if (is_castle(game->board, move)) {
                game->whiteCastled = true;
            }
        }

        game->whiteMoveCounter++;
    } else {
        // Black moved
        game->blackMoveCounter++;

        // Update castling flags
        if (!game->blackCastled && (game->blackCanCastleShort || game->blackCanCastleLong)) {
            if (board_value_from_shift(game->board->king, move->target)) {
                // King moved
                game->blackCanCastleShort = false;
                game->blackCanCastleLong = false;
            } else if (board_value_from_shift(game->board->rook, move->target) && position_from_shift(move->target).x == 7) {
                // Short rook moved
                game->blackCanCastleShort = false;
            } else if (board_value_from_shift(game->board->rook, move->target) && position_from_shift(move->target).x == 0) {
                game->blackCanCastleLong = false;
            }
            if (is_castle(game->board, move)) {
                game->blackCastled = true;
            }
        }
    }
}

void play() {
    t_game *game = startGame();
   // setFen(game->board, (char *)"r3k2r/8/8/8/8/8/p1r4r/R3K2R");

    printBoard(game->board);
    sleep(4);

    while (!game->isOver) {
      /*  if (game->blackMoveCounter > 0) {
            break;
        } */

        // sleep(1);

        // Generate next move
        t_move nextMove = getMove(game, game->turn);
        commitMove(game, &nextMove);

        // Print next move and resulting board state
        printf("Next move: ");
        printMove(nextMove);

        printf("Current board state (Round: %d, ", (game->blackMoveCounter + 1));

        if (game->turn == 0) {
            printf("Turn: White)\n");
        }
         else {
            printf("Turn: Black)\n");
         }


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
