#ifndef KINGOFTHEHILL_KI_END_H
#define KINGOFTHEHILL_KI_END_H


#include "game.h"
#include "moveMaps.h"


typedef enum {
    NOTOVER,
    WHITE,
    BLACK,
    DRAW
} winner_t;


template<bool color>
inline static bool isCheckmate(t_game *game) {
    /* Function to check whether the now moving party is checkmate
    * Arguments:
    *  t_board *board: Pointer to the board representing the state of the game
    *  bool color: the next moving color with "false" for white and "true" for black
    */

    t_board board = game->board();

    if (color) {
        bool kingThreatened = board.blackKing & getThreatened<color>(board);
        if (kingThreatened) {
            return generate_moves<color>(*game->state).empty();  // King is threatened and there are no moves
        }
    } else {
        bool kingThreatened = board.whiteKing & getThreatened<color>(board);
        if (kingThreatened) {
            return generate_moves<color>(*game->state).empty();  // King is threatened and there are no moves
        }
    }

    return false;
}

template<bool color>
inline static bool isStalemate(t_game *game) {
    /* Function to check whether the party whose turn is now can move. If not the game is stalemate-> draw
    * Arguments:
    *  t_board *board: Pointer to the board representing the state of the game
    *  bool color: the next moving color with "false" for white and "true" for black
    */
    std::vector<t_gameState> possibleMoves = generate_moves<color>(game->state);
    return possibleMoves.empty();
}


template<bool color>
inline static bool isKingOfTheHill(t_board *board) {
    /* Function to check whether King of the Hill condition is fulfilled
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the gameOld
     *  bool color: the last moved color with "false" for white and "true" for black
     */

    if (color) {
        return (board->blackKing & kingOfTheHill) != 0;
    } else {
        return (board->whiteKing & kingOfTheHill) != 0;
    }
}

template<bool color>
inline static winner_t checkEnd(t_game *game) {
    /* Function to check whether one of the gameOld ending conditions is fulfilled and Returns the winner or DRAW. Given is the last moved color
     * tho check if the last action finished the gameOld.
     * Arguments:
     *  t_gameOld *gameOld: Pointer to the gameOld representing the state of the gameOld
     *  bool moved_color: the last moved color with "false" for white and "true" for black
     */


    if (isKingOfTheHill<color>(game->board())) {
        return color ? BLACK : WHITE;
    } else if (isCheckmate<!color>(game)) {
        return color ? BLACK : WHITE;
    } else if (isStalemate<!color>(game)) {
        return DRAW;
    } else if (game->positionRepetitions()) {
        return DRAW;
    }

    return NOTOVER;
}


#endif //KINGOFTHEHILL_KI_END_H
